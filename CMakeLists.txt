cmake_minimum_required(VERSION 3.15)  # 建议使用较新的 CMake 版本

project(MainWindow LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 找到 Qt 模块
find_package(Qt6 COMPONENTS Core Gui Widgets Sql Concurrent Charts REQUIRED)

# Qt 设置 (在 find_package 之后立即调用)
qt_standard_project_setup()

# 启用 Qt 自动化工具 (建议在 qt_standard_project_setup 之后，尽管顺序影响不大)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(DATABASE_PATH "${CMAKE_SOURCE_DIR}/sql/ManageSys.db") #
message(STATUS "DATABASE_PATH: ${DATABASE_PATH}") # 调试输出

# 添加可执行文件
qt_add_executable(${PROJECT_NAME}
    main.cpp
    MainWindow.cpp
    MainWindow.ui  # CMake 会自动处理 .ui 文件
    MainWindow.qrc
    DataBaseManSys.cpp
    DataBaseManSys.h
    stuInfoWidget.cpp
    stuInfoWidget.ui
    Delegate.h
    ScheduleWidget.cpp
    FinancialWidget.cpp
     HonorWallWidget.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE DATABASE_PATH="${DATABASE_PATH}")

# 添加包含路径（如果 stuInfoWidget.h 不在项目根目录中，请修改路径）
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Concurrent
        Qt6::Charts
)

# 设置源文件属性 (如果需要在 IDE 中更好地组织文件)
set_source_files_properties(MainWindow.ui PROPERTIES UI_FILE TRUE) # 显式声明是 UI 文件
set_source_files_properties(MainWindow.h PROPERTIES HEADER_FILE_ONLY TRUE) # 显式声明是头文件，可选

#不再需要手动 include_directories 和 link_directories
#不再需要 set(PROJECT_SOURCES ...)

# 示例：添加自定义资源文件 (如果需要)
# add_executable(${PROJECT_NAME} MainWindow.qrc)
