## setup OS_LIBS
set (OS_LIBS)

if (MARSYAS_MACOSX)
	find_library (COREFOUNDATION_LIBRARY CoreFoundation)
	mark_as_advanced (COREFOUNDATION_LIBRARY)
	list (APPEND OS_LIBS ${COREFOUNDATION_LIBRARY})
	if (MARSYAS_AUDIOIO)
		find_library (COREAUDIO_LIBRARY CoreAudio)
		mark_as_advanced (COREAUDIO_LIBRARY)
		list (APPEND OS_LIBS ${COREAUDIO_LIBRARY})
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		find_library (COREMIDI_LIBRARY CoreMidi)
		mark_as_advanced (COREMIDI_LIBRARY)
		list (APPEND OS_LIBS ${COREMIDI_LIBRARY})
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MACOSX)

if (MARSYAS_LINUX)
	if (MARSYAS_AUDIOIO)
		find_library(ASOUND_LIBRARY asound)
		mark_as_advanced(ASOUND_LIBRARY)
		list (APPEND OS_LIBS ${ASOUND_LIBRARY})
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		find_library(ASOUND_LIBRARY asound)
		mark_as_advanced(ASOUND_LIBRARY)
		list (APPEND OS_LIBS ${ASOUND_LIBRARY})
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_LINUX)

if (MARSYAS_WIN32)
	if (MARSYAS_AUDIOIO)
#   set won't do anything in here.  -gp
#set(__WINDOWS_DS__ 1)
#set(MARSYAS_DS 1)
		find_library(DSOUND_LIBRARY dsound)
		mark_as_advanced(DSOUND_LIBRARY)
		list (APPEND OS_LIBS ${DSOUND_LIBRARY})
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_WIN32)

if (MARSYAS_MINGW)
	if (MARSYAS_AUDIOIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MINGW)

if (MARSYAS_CYGWIN)
	if (MARSYAS_AUDIOIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN)


