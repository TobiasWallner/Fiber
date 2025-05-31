#pragma once

#include <fiber/Interfaces/cPin.hpp>
#include <fiber/OS/Delay.hpp>

namespace fiber
{

    enum class LCD_HD44780_NumberOfLines : unsigned int {
        One = 0, 
        Two = 0b00001000
    };

    enum class LCD_HD44780_Dots : unsigned int {
        _5x8 = 0, 
        _5x11 = 0b00000100
    };

    enum class LCD_HD44780_CursorEntryDirection : unsigned int {
        Decrement = 0, 
        Increment = 0b00000010
    };

    enum class LCD_HD44780_DisplayEntryShift : unsigned int {
        Off = 0, 
        On = 0b00000001
    };

    enum class LCD_HD44780_DisplayControl : unsigned int {
        Off = 0, 
        On = 0b00000100
    };

    enum class LCD_HD44780_CursorControl : unsigned int {
        Off = 0, 
        On = 0b00000010, 
        Blink = 0b00000011
    };

    enum class LCD_HD44780_CursorDisplayShift : unsigned int {
        CursorLeft   = 0b00000000,
        CursorRight  = 0b00000100,
        DisplayLeft  = 0b00001000,
        DisplayRight = 0b00001100,
    };

    struct LCD_HD44780_BusyFlagAddress{
        uint8_t address;
        bool busy_flag;
    };

    static_assert(cAwaitable<Coroutine<void>>, "");

    /**
     * \brief Template interface for the 'LCD_HD447080' or compatible version
     */
    template<class LCD>
    concept cLCD_HD44780 = requires(
            LCD lcd, 
            LCD_HD44780_NumberOfLines number_of_lines, 
            LCD_HD44780_Dots dots, 
            bool boolean,
            LCD_HD44780_CursorEntryDirection direction, 
            LCD_HD44780_DisplayEntryShift shift,
            LCD_HD44780_DisplayControl display_control, 
            LCD_HD44780_CursorControl cursor_control,
            LCD_HD44780_CursorDisplayShift cursor_display_shift,
            uint8_t address)
    {
        { lcd.columns() } -> std::unsigned_integral;
        { lcd.rows() } -> std::unsigned_integral;

        { lcd.background_light(boolean) } -> cAwaitable<void>;
        { lcd.init() } -> cAwaitable<void>;
        { lcd.clear_display() } -> cAwaitable<void>;
        { lcd.return_home() } -> cAwaitable<void>;
        { lcd.entry_mode(direction, shift) } -> cAwaitable<void>;
        { lcd.display_control(display_control, cursor_control) } -> cAwaitable<void>;
        { lcd.cursor_and_display_shift(cursor_display_shift) } -> cAwaitable<void>;
        { lcd.function_set(number_of_lines, dots) } -> cAwaitable<void>;
        { lcd.set_cg_address(address) } -> cAwaitable<void>;
        { lcd.set_dd_address(address) } -> cAwaitable<void>;
        { lcd.write_data(address) } -> cAwaitable<void>;
        { lcd.read_data() } -> cAwaitable<uint8_t>;
        { lcd.read_busy_flag_and_address() } -> cAwaitable<LCD_HD44780_BusyFlagAddress>;
        { lcd.is_busy() } -> cAwaitable<bool>;
    };

    /**
     * \brief driver for a LCD display with 8 data bit interface
     * 
     * Implements the communication protocol for the **Hitachi HD44780** LCD controller.
     * 
     * Datasheet: https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
     * 
     * Compatible controllers:
     * 
     * | Controller       | Notes                                           |
     * |------------------|-------------------------------------------------|
     * | HD44780          | Original from Hitachi                           |
     * | KS0066U          | Samsung clone                                   |
     * | ST7066U          | Sitronix version                                |
     * | AIP31066         | Taiwan-based clone                              |
     * | SPLC780D         | Sitronix clone                                  |
     * | NT7605           | Novatek                                         |
     * | TC2004A-01       | Module series using compatible controller       |
     * | WH1602, WH2004   | WinStar modules with compatible controllers     |
     * 
     * 
     * \tparam DataPins InputOutput Data Pins D0..D7
     * \tparam RSPin Function Select
     * \tparam RWPin Read/Write select
     * \tparam EPin Enable/Clock Pin
     * \tparam APin Background Enable Pin (optional)
     */
    template<cOutPin RSPin, cOutPin RWPin, cOutPin EPin, cPins<8> DataPins, cOutPin APin = VoidPin>
    class LCD_HD44780{
    private:
        DataPins _data_pins;
        RSPin _rs_pin;
        RWPin _rw_pin;
        EPin _e_pin;
        APin _a_pin;

        unsigned int _columns;

        LCD_HD44780_NumberOfLines _lines;
        LCD_HD44780_Dots _dots;

    public:
        constexpr LCD_HD44780(
            LCD_HD44780_NumberOfLines lines, unsigned int columns, LCD_HD44780_Dots dots,
            const RSPin& rs_pin, const RWPin& rw_pin, const EPin& e_pin, 
            const DataPins& data_pins,  
            const APin& a_pin=VoidPin()
        )
            : _data_pins(data_pins)
            , _rs_pin(rs_pin)
            , _rw_pin(rw_pin)
            , _e_pin(e_pin)
            , _a_pin(a_pin)
            , _columns(columns)
            , _lines(lines)
            , _dots(dots){}

    public:

        /**
         * \brief returns the number of columns of the LCD display
         */
        constexpr unsigned int columns() const {return this->_columns;}

        /**
         * \brief returns the number or rows of the LCD display
         */
        constexpr unsigned int rows() const {
            if(LCD_HD44780_NumberOfLines::One == this->_lines){
                return 1;
            }else{
                return 2;
            }
        }

        [[nodiscard]] std::suspend_never background_light(bool b){
            this->_a_pin.write(b);
            return std::suspend_never{};
        }

        [[nodiscard]] Coroutine<void> init(){
            // set all output low
            this->_data_pins.dir(0);
            this->_rs_pin.dir(false);
            this->_rw_pin.dir(false);
            this->_e_pin.dir(false);
            this->_a_pin.dir(false);

            this->_data_pins.write(0);
            this->_rs_pin.write(false);
            this->_rw_pin.write(false);
            this->_e_pin.write(false);
            this->_a_pin.write(false);

            co_await Delay(50ms);
            
            // function set command
            {
                const unsigned int command_flag = 0b00100000;
                const unsigned int dataline_flag = 0b00010000; // 8 bit interface
                const unsigned int command = command_flag | dataline_flag | static_cast<unsigned int>(_lines) | static_cast<unsigned int>(_dots);
                co_await this->write_command_no_wait(command);
            }

            co_await Delay(50us);

            // display control
            {
                const unsigned int command_flag = 0b00001000;
                const unsigned int command = command_flag 
                    | static_cast<unsigned int>(LCD_HD44780_DisplayControl::On) 
                    | static_cast<unsigned int>(LCD_HD44780_CursorControl::Off);
                co_await this->write_command_no_wait(command);
            }

            co_await Delay(50us);

            // display clear
            {
                const unsigned int command_flag = 0b00000001;
                co_await this->write_command_no_wait(command_flag);
            }

            co_await Delay(2ms);

            // entry mode
            {
                const unsigned int command_flag = 0b00000100;
                const unsigned int command = command_flag 
                    | static_cast<unsigned int>(LCD_HD44780_CursorEntryDirection::Increment) 
                    | static_cast<unsigned int>(LCD_HD44780_DisplayEntryShift::Off);
                co_await this->write_command_no_wait(command);
            }
        }

        /**
         * \brief Clears the display and returns the cursor to the home position (address 0)
         * */
        [[nodiscard]] Coroutine<void> clear_display(){
            const unsigned int command_flag = 0b00000001;
            return this->write_command(command_flag);
        }

        /**
         * \brief Returns the cursor to the home position (address 0)
         * 
         * Also returns a shifted display to the home position.
         * DDRAM Contents remain Unchanged.
         */
        [[nodiscard]] Coroutine<void> return_home(){
            const unsigned int command_flag = 0b00000010;
            return this->write_command(command_flag);
        }

        
        /**
         * \brief Set the cursors move direction and display shift
         */
        [[nodiscard]] Coroutine<void> entry_mode(LCD_HD44780_CursorEntryDirection direction, LCD_HD44780_DisplayEntryShift shift){
            const unsigned int command_flag = 0b00000100;
            const unsigned int command = command_flag | static_cast<unsigned int>(direction) | static_cast<unsigned int>(shift);
            return this->write_command(command);
        }

        /**
         * \brief Turn the display on/off and the curser on/off/blink
         */
        [[nodiscard]] Coroutine<void> display_control(LCD_HD44780_DisplayControl display_control, LCD_HD44780_CursorControl cursor_control){
            const unsigned int command_flag = 0b00001000;
            const unsigned int command = command_flag | static_cast<unsigned int>(display_control) | static_cast<unsigned int>(cursor_control);
            return this->write_command(command);
        }

        

        [[nodiscard]] Coroutine<void> cursor_and_display_shift(LCD_HD44780_CursorDisplayShift cursor_display_shift){
            const unsigned int command_flag = 0b00010000;
            const unsigned int command = command_flag | static_cast<unsigned int>(cursor_display_shift);
            return this->write_command(command);
        }
        
        /**
         * \brief set the data width, the number of lines and the character font
         */
        [[nodiscard]] Coroutine<void> function_set(LCD_HD44780_NumberOfLines number_of_lines, LCD_HD44780_Dots dots){
            const unsigned int command_flag = 0b00100000;
            const unsigned int dataline_flag = 0b00010000; // 8 bit interface
            const unsigned int command = command_flag | dataline_flag | static_cast<unsigned int>(number_of_lines) | static_cast<unsigned int>(dots);
            return this->write_command(command);
        }

        /**
         * \brief set the cg ram address (6-bit)
         */
        [[nodiscard]] Coroutine<void> set_cg_address(uint8_t addr){
            const unsigned int command_flag = 0b01000000;
            const unsigned int command = command_flag | (addr & 0b00111111);
            return this->write_command(command);
        }

        /**
         * \brief set the dd ram address (7-bit)
         */
        [[nodiscard]] Coroutine<void> set_dd_address(uint8_t addr){
            const unsigned int command_flag = 0b10000000;
            const unsigned int command = command_flag | (addr & 0b01111111);
            return this->write_command(command);
        }

        /**
         * \brief write data into the 'dd' or 'cg' ram
         */
        [[nodiscard]] Coroutine<void> write_data(uint8_t data){
            return this->write(true, data);
        }

        /**
         * \brief Read data from the 'dd' or 'cg' ram 
         */
        [[nodiscard]]Coroutine<uint8_t> read_data(){
            return this->read(true);
        }

        [[nodiscard]] Coroutine<LCD_HD44780_BusyFlagAddress> read_busy_flag_and_address(){
            uint8_t data = co_await this->read_no_wait(false);
            LCD_HD44780_BusyFlagAddress result;
            result.address   =  data & 0b01111111;
            result.busy_flag = (data & 0b10000000) != 0;
            co_return result;
        }

        [[nodiscard]] Coroutine<bool> is_busy(){
            uint8_t data = co_await this->read_no_wait(false);
            const bool result = (data & 0b10000000) != 0;
            co_return result;
        }

    private:

        [[nodiscard]] Coroutine<void> wait_while_busy(){
            // wait until the display is ready
            while(true){
                const bool busy = co_await is_busy();
                if(busy){
                    co_await Delay(10us);
                }else{
                    break;
                }
            }
        }

        [[nodiscard]] Coroutine<void> write_command(std::bitset<8> command){
            return this->write(false, command);
        }

        [[nodiscard]] Coroutine<void> write_command_no_wait(std::bitset<8> command){
            return this->write_no_wait(false, command);
        }

        [[nodiscard]] Coroutine<void> write(bool rs, std::bitset<8> command){
            // wait until the lcd is ready to receive new commands
            co_await this->wait_while_busy();

            co_await write_no_wait(rs, command);
        }

        [[nodiscard]] Coroutine<void> write_no_wait(bool rs, std::bitset<8> command){
            // configure pins to output for writing
            this->_data_pins.dir(0);

            // config to write command and writing mode
            this->_rs_pin.write(rs); // rs=false: command, rs=true: data
            this->_rw_pin.low(); // writing
            co_await fiber::Delay(1us); // setup/hold time

            // set enable flag
            this->_e_pin.high();
            co_await fiber::Delay(1us); // setup/hold time

            // set data
            this->_data_pins.write(command);
            co_await fiber::Delay(1us); // setup/hold time

            // clear enable flag
            this->_e_pin.low();
            co_await fiber::Delay(1us); // setup/hold time
        }

        [[nodiscard]]Coroutine<uint8_t> read(bool rs){
            // wait until the lcd is ready to receive new commands
            co_await this->wait_while_busy();   

            const uint8_t result_data = co_await read_no_wait(rs);
            co_return result_data;
        }

        [[nodiscard]]Coroutine<uint8_t> read_no_wait(bool rs){      
            // set data pins to input for reading
            this->_data_pins.dir(0xFF);

            // config pins for data and reading
            this->_rs_pin.write(rs); // data
            this->_rw_pin.high(); // reading
            co_await fiber::Delay(1us); // setup/hold time

            // set enable flag
            this->_e_pin.high();
            co_await fiber::Delay(1us); // setup/hold time

            // read data
            const std::bitset<8> result_data_bits = this->_data_pins.read();
            const uint8_t result_data = static_cast<uint8_t>(result_data_bits.to_ulong());

            this->_e_pin.low();
            co_await fiber::Delay(1us); // setup/hold time

            co_return result_data;
        }
    }; static_assert(cLCD_HD44780<LCD_HD44780<VoidPin, VoidPin, VoidPin, VoidPins<8>, VoidPin>>, "LCD_HD44780 should implement cLCD_HD44780");
} // names