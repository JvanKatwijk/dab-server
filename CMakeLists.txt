cmake_minimum_required( VERSION 2.8.11 )
set (objectName dab_cmdline-7)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()
#set (CMAKE_INSTALL_PREFIX /usr/local/bin)


if(MINGW)
    add_definitions ( -municode)
endif()

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#
########################################################################
if(DEFINED AIRSPY)
   set(AIRSPY true)
endif ()

if(DEFINED SDRPLAY)
   set(SDRPLAY true)
endif ()

if(DEFINED RTLSDR)
   set(RTLSDR true)
endif ()

if(DEFINED HACKRF)
    set(HACKRF true)
endif ()

set(objectName dab-server)


#########################################################################
	find_package (PkgConfig)

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()

        find_package(Faad)
        if (NOT FAAD_FOUND )
            message(FATAL_ERROR "please install libfaad")
        endif ()

        find_package(zlib)
	if (NOT ZLIB_FOUND)
            message(FATAL_ERROR "please install libz")
        endif ()
	list(APPEND extraLibs ${ZLIB_LIBRARY})


	find_library (PTHREADS pthread)
	if (NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else (NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif (NOT(PTHREADS))


        find_package(LibSampleRate)
        if (NOT LIBSAMPLERATE_FOUND)
            message(FATAL_ERROR "please install libsamplerate")
        endif ()
        list(APPEND extraLibs ${LIBSAMPLERATE_LIBRARY})

        find_package(Portaudio)
        if (NOT PORTAUDIO_FOUND)
            message(FATAL_ERROR "please install portaudio V19")
        endif ()
        list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})
#########################################################################
        find_package (PkgConfig)

##########################################################################
#	The devices
#


	if (SDRPLAY)
	   find_path (SDRPLAYLIB_INCLUDE_DIR
                      NAMES mirsdrapi-rsp.h
                      PATHS
                      /usr/local/include/
	   )
           include_directories (${SDRPLAYLIB_INCLUDE_DIR})

           find_library (SDRPLAYLIB mirsdrapi-rsp)
           if(NOT(SDRPLAYLIB))
              message(FATAL_ERROR "please install -lmirsdrapi-rsp")
           else(NOT(SDRPLAYLIB))
              list (APPEND extraLibs ${SDRPLAYLIB})
           endif(NOT(SDRPLAYLIB))

	   include_directories (
	     ./devices/sdrplay-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/sdrplay-handler/sdrplay-handler.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/sdrplay-handler/sdrplay-handler.cpp
	   )

	 add_definitions (-DHAVE_SDRPLAY)
	endif (SDRPLAY)

	if (AIRSPY)
           find_library (AIRSPYLIB airspy)
           if(NOT(AIRSPYLIB))
              message(FATAL_ERROR "please install -lairspy")
           else(NOT(AIRSPYLIB))
              list (APPEND extraLibs ${AIRSPYLIB})
           endif(NOT(AIRSPYLIB))
           find_package(LibAIRSPY)
           if (NOT LIBAIRSPY_FOUND)
               message(FATAL_ERROR "please install airspy library")
           endif ()
	   ### include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   include_directories (
	     ./devices/airspy-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/airspy-handler/airspy-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/airspy-handler/airspy-handler.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY)
	endif (AIRSPY)


	if (RTLSDR)
           find_library (RTLSDRLIB rtlsdr)
           if(NOT(RTLSDRLIB))
              message(FATAL_ERROR "please install -lrtlsdr")
           else(NOT(RTLSDRLIB))
              list (APPEND extraLibs ${RTLSDRLIB})
           endif(NOT(RTLSDRLIB))
           find_package(LibRTLSDR)
           if (NOT LIBRTLSDR_FOUND)
               message(FATAL_ERROR "please install librtlsdr")
           endif ()
	   include_directories (${RTLSDR_INCLUDE_DIR})

	   include_directories (
	        ./devices/rtlsdr-handler/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()

	if (HACKRF)
           find_path (HACKLIB_INCLUDE_DIR
                      NAMES hackrf.h
                      PATHS
                      ./devices/hackrf-handler/libhackrf/
                       /usr/local/include/
           )
           include_directories (${HACKLIB_INCLUDE_DIR})

           find_library (HACKLIB hackrf)
           if(NOT(HACKLIB))
              message(FATAL_ERROR "please install -lhackrf")
           else(NOT(HACKLIB))
              list (APPEND extraLibs ${HACKLIB})
           endif(NOT(HACKLIB))

           include_directories (
             ./devices/hackrf-handler
           )

           set ($(objectName)_HDRS
                ${${objectName}_HDRS}
                ./devices/hackrf-handler/hackrf-handler.h
           )

           set (${objectName}_SRCS
                ${${objectName}_SRCS}
                ./devices/hackrf-handler/hackrf-handler.cpp
           )

         add_definitions (-DHAVE_HACKRF)
        endif (HACKRF)

#######################################################################
#
#	Here we really start

	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           .
	           ./
	           ../
	           ./devices
	           ./radio
	           ./radio/includes
	           ./radio/includes/ofdm
	           ./radio/includes/backend
	           ./radio/includes/backend/audio
	           ./radio/includes/backend/data
	           ./radio/includes/backend/data/mot
	           ./radio/includes/backend/data/journaline
	           ./radio/includes/support
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ${${objectName}_HDRS}
	     ./protocol.h
	     ./radio/config.h
	     ./radio/radiodata.h
	     ./radio/dab-decoder.h
	     ./radio/includes/support/ringbuffer.h
	     ./radio/includes/support/band-handler.h
	     ./radio/includes/support/audio-base.h
	     ./radio/includes/support/audiosink.h
	     ./radio/includes/support/newconverter.h
	     ./radio/includes/dab-constants.h
	     ./radio/includes/dab-processor.h
	     ./radio/includes/ofdm/phasereference.h
	     ./radio/includes/ofdm/phasetable.h
	     ./radio/includes/ofdm/freq-interleaver.h
	     ./radio/includes/ofdm/timesyncer.h
	     ./radio/includes/ofdm/fic-handler.h
	     ./radio/includes/ofdm/fib-processor.cpp
	     ./radio/includes/ofdm/sample-reader.h
	     ./radio/includes/backend/firecode-checker.h
	     ./radio/includes/backend/backend-base.h
	     ./radio/includes/backend/charsets.h
	     ./radio/includes/backend/galois.h
	     ./radio/includes/backend/reed-solomon.h
	     ./radio/includes/backend/msc-handler.h
	     ./radio/includes/backend/virtual-backend.h
	     ./radio/includes/backend/audio-backend.h
	     ./radio/includes/backend/data-backend.h
	     ./radio/includes/backend/audio/faad-decoder.h
	     ./radio/includes/backend/audio/mp4processor.h 
	     ./radio/includes/backend/audio/mp2processor.h 
	     ./radio/includes/backend/data/virtual-datahandler.h 
	     ./radio/includes/backend/data/tdc-datahandler.h 
	     ./radio/includes/backend/data/pad-handler.h 
	     ./radio/includes/backend/data/data-processor.h
	     ./radio/includes/backend/data/mot/mot-handler.h 
	     ./radio/includes/backend/data/mot/mot-dir.h 
	     ./radio/includes/backend/data/mot/mot-object.h 
	     ./radio/includes/support/viterbi-handler.h
	     ./radio/includes/support/protTables.h
	     ./radio/includes/support/protection.h
	     ./radio/includes/support/uep-protection.h
	     ./radio/includes/support/eep-protection.h
	     ./radio/includes/support/fft_handler.h
	     ./radio/includes/support/dab-params.h
	     ./radio/includes/support/tii_table.h
	     ./radio/devices/device-handler.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./radio/config.cpp
	     ./radio/dab-decoder.cpp
	     ./radio/src/support/audio-base.cpp
	     ./radio/src/support/audiosink.cpp
	     ./radio/src/support/newconverter.cpp
	     ./radio/src/support/band-handler.cpp
	     ./radio/src/dab-processor.cpp
	     ./radio/src/ofdm/ofdm-decoder.cpp
	     ./radio/src/ofdm/phasereference.cpp
	     ./radio/src/ofdm/phasetable.cpp
	     ./radio/src/ofdm/freq-interleaver.cpp
	     ./radio/src/ofdm/timesyncer.cpp
	     ./radio/src/ofdm/sample-reader.cpp
	     ./radio/src/ofdm/fib-processor.cpp
	     ./radio/src/ofdm/fic-handler.cpp
	     ./radio/src/backend/firecode-checker.cpp
	     ./radio/src/backend/backend-base.cpp
	     ./radio/src/backend/charsets.cpp
	     ./radio/src/backend/galois.cpp
	     ./radio/src/backend/reed-solomon.cpp
	     ./radio/src/backend/msc-handler.cpp
	     ./radio/src/backend/virtual-backend.cpp
	     ./radio/src/backend/audio-backend.cpp
	     ./radio/src/backend/data-backend.cpp
	     ./radio/src/backend/audio/faad-decoder.cpp
	     ./radio/src/backend/audio/mp4processor.cpp 
	     ./radio/src/backend/audio/mp2processor.cpp 
	     ./radio/src/backend/data/virtual-datahandler.cpp 
	     ./radio/src/backend/data/tdc-datahandler.cpp 
	     ./radio/src/backend/data/pad-handler.cpp 
	     ./radio/src/backend/data/data-processor.cpp
	     ./radio/src/backend/data/mot/mot-handler.cpp 
	     ./radio/src/backend/data/mot/mot-dir.cpp 
	     ./radio/src/backend/data/mot/mot-object.cpp 
	     ./radio/src/support/viterbi-handler.cpp
	     ./radio/src/support/protTables.cpp
	     ./radio/src/support/protection.cpp
	     ./radio/src/support/eep-protection.cpp
	     ./radio/src/support/uep-protection.cpp
	     ./radio/src/support/fft_handler.cpp
	     ./radio/src/support/dab-params.cpp
	     ./radio/src/support/tii_table.cpp
	     ./devices/device-handler.cpp
	)
#
	include_directories (
	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

#####################################################################

	add_executable (${objectName} 
	                ${${objectName}_SRCS}
	)

	target_link_libraries (${objectName}
	                       ${FFTW3F_LIBRARIES}
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	                       -lbluetooth
	)

	INSTALL (TARGETS ${objectName} DESTINATION .)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
