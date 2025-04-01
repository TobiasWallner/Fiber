#pragma once

namespace embed{

    enum class Exit {
        Success,   ///< Task completed successfully; destroy it
        Failure    ///< Task failed; notify or destroy
    };

}