cmake_minimum_required(VERSION 3.21)
project(ZeebrooPosDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Multimedia PrintSupport)

qt_add_executable(ZeebrooPosDesktop
    src/main.cpp
    src/core/Config.cpp
    src/core/Models.cpp
    src/core/ApiClient.cpp
    src/core/Cart.cpp
    src/core/PosBeep.cpp
    src/core/SaleReceipt.cpp
    src/ui/LoginDialog.cpp
    src/ui/MainWindow.cpp
    src/ui/ProductCardWidget.cpp
    src/ui/CartLineWidget.cpp
    src/ui/LayerPickerDialog.cpp
    src/ui/ReceiptDialog.cpp
    resources/resources.qrc
)

target_include_directories(ZeebrooPosDesktop PRIVATE src)

target_link_libraries(ZeebrooPosDesktop PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::PrintSupport
)

if(WIN32)
    set_target_properties(ZeebrooPosDesktop PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS ZeebrooPosDesktop RUNTIME DESTINATION bin)
