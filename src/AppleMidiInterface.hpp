/*!
 *	@brief		AppleMIDI Library for the Arduino
 *  @author		lathoub, hackmancoltaire, chris-zen
 */

BEGIN_APPLEMIDI_NAMESPACE

/*! \brief Default constructor for MIDI_Class. */
template<class UdpClass, class Settings>
inline AppleMidiInterface<UdpClass, Settings>::AppleMidiInterface()
{
#if APPLEMIDI_USE_CALLBACKS
	// Initialise callbacks to NULL pointer
	mConnectedCallback				= NULL;
	mDisconnectedCallback			= NULL;
#endif

	// initiative to 0, the actual SSRC will be generated lazily
	_ssrc = 0;

	uint32_t initialTimestamp_ = 0;
	_rtpMidiClock.Init(initialTimestamp_, MIDI_SAMPLING_RATE_DEFAULT);
}

/*! \brief Default destructor for MIDI_Class.

 This is not really useful for the Arduino, as it is never called...
 */
template<class UdpClass, class Settings>
inline AppleMidiInterface<UdpClass, Settings>::~AppleMidiInterface()
{
}

// ------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------

#if APPLEMIDI_USE_CALLBACKS

template<class UdpClass, class Settings> inline void AppleMidiInterface<UdpClass, Settings>::OnConnected(void(*fptr)(uint32_t, char*))    { mConnectedCallback = fptr; }
template<class UdpClass, class Settings> inline void AppleMidiInterface<UdpClass, Settings>::OnDisconnected(void(*fptr)(uint32_t))      { mDisconnectedCallback = fptr; }

#endif

END_APPLEMIDI_NAMESPACE
