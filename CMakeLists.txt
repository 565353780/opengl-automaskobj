#cmake版本
cmake_minimum_required(VERSION 3.5.1)

#项目名称
project(opengl_auto_mask_obj)

#让cmake自动去寻找Qt5
FIND_PACKAGE(Qt5 COMPONENTS widgets core gui opengl REQUIRED)

#采用C++11标准
set(CMAKE_CXX_STANDARD 17)

#添加所有的源文件
SET(${PROJECT_NAME}_SOURCES
  src/DataRebuild.cpp
)

#添加所有的头文件
SET(${PROJECT_NAME}_HEADERS
  include/DataRebuild.h
)

#添加所有的.ui文件
SET(${PROJECT_NAME}_FORMS
  mainwindow.ui
)

#添加资源文件
SET(${PROJECT_NAME}_RESOURCES
	img.qrc
)

#调用预编译器moc，需要使用 QT5_WRAP_CPP宏
QT5_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOC ${${PROJECT_NAME}_HEADERS})
#使用uic处理.ui文件
QT5_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
#使用rcc处理.qrc文件
QT5_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})

# 这些生成的中间文件都会在build目录下
# 这样的话，编译器则不能定位由uic程序产生的诸如_ui_mainwindow.h等文件
# 所以，我们需要把build目录添加到包含目录中
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#生成可执行文件
ADD_EXECUTABLE(${PROJECT_NAME}
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_FORMS_HEADERS}
	${${PROJECT_NAME}_RESOURCES_RCC}
)

#为target添加需要链接的共享库
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES})

