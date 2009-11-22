# Option to produce condor executables
OPTION(BUILD_CONDOR_EXE "Build condor executables" OFF)
MARK_AS_ADVANCED(BUILD_CONDOR_EXE)
IF (BUILD_CONDOR_EXE)
   SET (CMAKE_X_LIBS "-lGL -L/usr/X11R6/lib -lm -lXext -lXt -lSM -lX11  -lICE -ldl -lnsl")
   SET (CMAKE_MODULE_LINK_FLAGS "-static")
   SET (CMAKE_SHLIB_LINK_FLAGS "-static")
ENDIF (BUILD_CONDOR_EXE)

# Option to produce multi-threaded executables using TBB
OPTION(BUILD_TBB_EXE "Build multi-threaded executables using TBB" OFF)
MARK_AS_ADVANCED(BUILD_TBB_EXE)

IF (BUILD_TBB_EXE)
   ADD_DEFINITIONS(-DHAS_TBB)
   SET(TBB_DIR "" CACHE PATH "Path to where TBB is installed")
   SET(TBB_INCLUDE_DIR ${TBB_DIR}/include)
   SET(TBB_LIB_DIR ${TBB_DIR}/lib)
   INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIR})
   LINK_DIRECTORIES(${TBB_LIB_DIR})
   LINK_LIBRARIES(tbb)
ENDIF(BUILD_TBB_EXE)

INCLUDE(${CMAKE_ROOT}/Modules/FindZLIB.cmake)

IF (ZLIB_FOUND)
  ADD_DEFINITIONS(-DHAS_ZLIB)
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
  LINK_LIBRARIES(${ZLIB_LIBRARIES})
ENDIF (ZLIB_FOUND)

INCLUDE(${CMAKE_ROOT}/Modules/FindPNG.cmake)

INCLUDE(${CMAKE_ROOT}/Modules/FindFLTK.cmake)

IF (FLTK_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${FLTK_INCLUDE_PATH})
ENDIF(FLTK_INCLUDE_PATH)

IF (FLTK_LIBRARIES)
  LINK_LIBRARIES (${FLTK_LIBRARIES})
ENDIF (FLTK_LIBRARIES)

IF (NOT BUILD_CONDOR_EXE)

   INCLUDE (${CMAKE_ROOT}/Modules/FindGLUT.cmake)

   IF (GLUT_INCLUDE_PATH)
      INCLUDE_DIRECTORIES(${GLUT_INCLUDE_PATH})
   ENDIF(GLUT_INCLUDE_PATH)

   IF (GLUT_LIBRARY)
      LINK_LIBRARIES (${GLUT_LIBRARY})
   ENDIF (GLUT_LIBRARY)

   INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)

   IF (GLU_LIBRARY)
      LINK_LIBRARIES (${GLU_LIBRARY})
   ENDIF (GLU_LIBRARY)
 
   IF (OPENGL_INCLUDE_PATH)
      INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
   ENDIF(OPENGL_INCLUDE_PATH)

   IF (OPENGL_LIBRARY)
      LINK_LIBRARIES (${OPENGL_LIBRARY})
   ENDIF (OPENGL_LIBRARY)

ENDIF (NOT BUILD_CONDOR_EXE)

INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

IF (WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /TP /Ze /W0")
  ADD_DEFINITIONS(-DvtkCommon_EXPORTS)
ELSE (WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffloat-store -Wall -O2")
ENDIF (WIN32)

# Option to wrap python.
OPTION(WRAP_PYTHON "Generate python wrapper libraries" OFF)

IF (WRAP_PYTHON)
  SET(ENABLE_WRAPPING TRUE)
  INCLUDE(${CMAKE_ROOT}/Modules/FindPythonLibs.cmake)
ENDIF (WRAP_PYTHON)

# Find SWIG.
IF (ENABLE_WRAPPING)
  INCLUDE(${CMAKE_ROOT}/Modules/FindSWIG.cmake)
  IF (SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})  
  ENDIF (SWIG_FOUND)
ENDIF (ENABLE_WRAPPING)

ADD_DEFINITIONS(-DIMPERIAL -DANSI -DHAS_CONTRIB -DNO_BOUNDS -DENABLE_UNIX_COMPRESS)

INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/recipes/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/common++/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/geometry++/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/image++/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/contrib++/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/packages/transformation/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/packages/registration/include)
INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/packages/segmentation/include)

LINK_DIRECTORIES(${IRTK_BINARY_DIR}/lib) 

# Option to build with PNG or not.
OPTION(BUILD_WITH_PNG "Build using PNG" OFF)

IF (BUILD_WITH_PNG)
  ADD_DEFINITIONS(-DHAS_PNG)
  INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
  LINK_LIBRARIES(${PNG_LIBRARIES})
ENDIF (BUILD_WITH_PNG)

# Option to build with VTK or not.
OPTION(BUILD_WITH_VTK "Build using VTK" OFF)

IF (BUILD_WITH_VTK)
   # Add VTK
   INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)

   IF (VTK_FOUND)
      ADD_DEFINITIONS(-DHAS_VTK)
      INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
      LINK_DIRECTORIES(${VTK_LIBRARY_DIRS})

      # Add patented library if available
      IF (VTK_KITS MATCHES "PATENTED")
         ADD_DEFINITIONS(-DHAS_VTK_PATENTED)
	  LINK_LIBRARIES (vtkPatented)
      ENDIF (VTK_KITS MATCHES "PATENTED")

       # Add patented library if available
      IF (VTK_KITS MATCHES "HYBRID")
         ADD_DEFINITIONS(-DHAS_VTK_HYBRID)
	 LINK_LIBRARIES (vtkHybrid)
      ENDIF (VTK_KITS MATCHES "HYBRID")

     LINK_LIBRARIES (vtkRendering vtkImaging
      vtkGraphics vtkFiltering vtkIO vtkCommon)
   ENDIF (VTK_FOUND)
ENDIF (BUILD_WITH_VTK)


LINK_LIBRARIES(segmentation++ registration++ transformation++ contrib++
image++ geometry++ common++ recipes) 

# Options to build with nifti, znz and possibly fslio
OPTION(BUILD_WITH_NIFTI "Build using NIFTI support" ON)
IF (BUILD_WITH_NIFTI)
   ADD_DEFINITIONS(-DHAS_NIFTI)
   INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/nifti/niftilib)
   INCLUDE_DIRECTORIES(${IRTK_SOURCE_DIR}/nifti/znzlib)
   LINK_LIBRARIES(znz)
   LINK_LIBRARIES(niftiio)
ENDIF (BUILD_WITH_NIFTI)