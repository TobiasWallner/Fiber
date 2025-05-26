#pragma once

#include <fiber/Interfaces/cPin.hpp>
#include <fiber/OS/Delay.hpp>

namespace fiber
{

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
    template<cPins<8> DataPins, cOutputPin RSPin, cOutputPin RWPin, cOutputPin EPin, cOutputPin APin = VoidPin>
    class LCD_HD44780{
    private:
        DataPins _data_pins;
        RSPin _rs_pin;
        RWPin _rw_pin;
        EPin _e_pin;
        APin _a_pin;

        unsigned int _columns;
        unsigned int _rows;

    public:
        LCD_HD44780(
            DataPins& data_pins, RSPin& rs_pin, RWPin& rw_pin, EPin& e_pin, APin& a_pin,
            unsigned int columns, unsigned int rows
        )
            : _data_pins(data_pins)
            , _rs_pin(rs_pin)
            , _rw_pin(rw_pin)
            , _e_pin(e_pin)
            , _a_pin(a_pin)
            , _columns(columns)
            , _rows(rows){}

    public:

        /**
         * \brief returns the number of columns of the LCD display
         */
        unsigned int columns() const {return this->_columns;}

        /**
         * \brief returns the number or rows of the LCD display
         */
        unsigned int rows() const {return this->_rows;}

        std::suspend_never background_light(bool b){
            this->_a_pin.set(b);
            return std::suspend_never{};
        }

        [[nodiscard]] Coroutine<void> init(NumberOfLines number_of_lines, Dots dots, bool background_light){
            co_await Delay<RoundingMethod::Up>(50ms);
            
            // function set command
            {
                const unsigned int command_flag = 0b00100000;
                const unsigned int dataline_flag = 0b00010000; // 8 bit interface
                co_await this->write_command_no_wait(command_flag | dataline_flag | number_of_lines | dots);
            }

            co_await Delay<RoundingMethod::Up>(50us);

            // display control
            {
                const unsigned int command_flag = 0b00001000;
                co_await this->write_command_no_wait(command_flag | DisplayControl::On | CursorControl::Off);
            }

            co_await Delay<RoundingMethod::Up>(50us);

            // display clear
            {
                const unsigned int command_flag = 0b00000001;
                return this->write_command_no_wait(command_flag);
            }

            co_await Delay<RoundingMethod::Up>(2ms);

            // entry mode
            {
                const unsigned int command_flag = 0b00000100;
                return this->write_command_no_wait(command_flag | CursorEntryDirection::Increment | DisplayEntryShift::Off);
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

        enum class CursorEntryDirection : unsigned int {Decrement = 0, Increment = 0b00000010};
        enum class DisplayEntryShift : unsigned int {Off = 0, On = 0b00000001};

        /**
         * \brief Set the cursors move direction and display shift
         */
        [[nodiscard]] Coroutine<void> entry_mode(CursorEntryDirection direction, DisplayEntryShift shift){
            const unsigned int command_flag = 0b00000100;
            return this->write_command(command_flag | direction | shift);
        }

        enum class DisplayControl : unsigned int {Off = 0, On = 0b00000100};
        enum class CursorControl : unsigned int {Off = 0, On = 0b00000010, Blink = 0b00000011};

        /**
         * \brief Turn the display on/off and the curser on/off/blink
         */
        [[nodiscard]] Coroutine<void> display_control(DisplayControl display_control, CursorControl cursor_control){
            const unsigned int command_flag = 0b00001000;
            return this->write_command(command_flag | display_control | cursor_control);
        }

        enum class CursorDisplayShift : unsigned int {
            CursorLeft   = 0b00000000,
            CursorRight  = 0b00000100,
            DisplayLeft  = 0b00001000,
            DisplayRight = 0b00001100,
        }

        [[nodiscard]] Coroutine<void> cursor_and_display_shift(CursorDisplayShift cursor_display_shift){
            const unsigned int command_flag = 0b00010000;
            return this->write_command(command_flag | cursor_display_shift);
        }

        enum class NumberOfLines{One = 0, Two = 0b00001000};
        enum class Dots{_5x8 = 0, _5x11 = 0b00000100};
        
        /**
         * \brief set the data width, the number of lines and the character font
         */
        [[nodiscard]] Coroutine<void> function_set(NumberOfLines number_of_lines, Dots dots){
            const unsigned int command_flag = 0b00100000;
            const unsigned int dataline_flag = 0b00010000; // 8 bit interface
            return this->write_command(command_flag | dataline_flag | number_of_lines | dots);
        }

        /**
         * \brief set the cg ram address (6-bit)
         */
        [[nodiscard]] Coroutine<void> set_cg_address(uint8_t addr){
            const unsigned int command_flag = 0b01000000;
            return this->write_command(command_flag | (addr & 0b00111111));
        }

        /**
         * \brief set the dd ram address (7-bit)
         */
        [[nodiscard]] Coroutine<void> set_dd_address(uint8_t addr){
            const unsigned int command_flag = 0b10000000;
            return this->write_command(command_flag | (addr & 0b01111111));
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

        struct BusyFlagAddress{
            uint8_t address;
            bool busy_flag;
        };

        [[nodiscard]] Coroutine<busy_flag_and_address> read_busy_flag_and_address(){
            uint8_t data = co_await this->read_no_wait(false);
            BusyFlagAddress result;
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
                    co_await Delay<RoundingMethod::Up>(10us);
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
            this->_rs_pin.set(rs); // rs=false: command, rs=true: data
            this->_rw_pin.low(); // writing
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            // set enable flag
            this->_e_pin.high();
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            // set data
            this->_data_pins.set(command);
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            // clear enable flag
            this->_e_pin.low();
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time
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
            this->_rs_pin.set(rs); // data
            this->_rw_pin.high(); // reading
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            // set enable flag
            this->_e_pin.high();
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            // read data
            const std::bitset<8> result_data_bits = this->_data_pins.read();
            const uint8_t result_data = static_cast<uint8_t>(result_data_bits.to_ulong());

            this->_e_pin.low();
            co_await fiber::Delay<RoundingMethod::Up>(1us); // setup/hold time

            co_return result_data;
        }

    };
} // names