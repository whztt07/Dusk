# Find VORBISFILE library
#
# VORBISFILE_INCLUDE_DIR      where to find the include files - will not be set
# VORBISFILE_LIBRARY_DIR      where to find the libraries
# VORBISFILE_LIBRARIES        list of libraries to link
# VORBISFILE_FOUND            true if VORBISFILE was found

SET( VORBISFILE_LIBRARYDIR / CACHE PATH "Alternative library directory" )
SET( VORBISFILE_INCLUDEDIR / CACHE PATH "Alternative include directory" )
MARK_AS_ADVANCED( VORBISFILE_LIBRARYDIR VORBISFILE_INCLUDEDIR )

FIND_LIBRARY( VORBISFILE_LIBRARY_DIR vorbisfile PATHS ${VORBISFILE_LIBRARYDIR} )
#FIND_PATH( VORBISFILE_INCLUDE_DIR vorbis/vorbisfile.h PATHS ${VORBISFILE_INCLUDEDIR} )

GET_FILENAME_COMPONENT( VORBISFILE_LIBRARY_DIR ${VORBISFILE_LIBRARY_DIR} PATH )

IF( VORBISFILE_LIBRARY_DIR )
    SET( VORBISFILE_LIBRARIES vorbisfile vorbis m ogg )
    SET( VORBISFILE_FOUND TRUE )
ELSE( VORBISFILE_LIBRARY_DIR )
    SET( VORBISFILE_FOUND FALSE )
ENDIF( VORBISFILE_LIBRARY_DIR )