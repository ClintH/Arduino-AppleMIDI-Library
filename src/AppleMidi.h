/*!
 *	@brief		RtpMIDI Library for the Arduino
 *  @author		lathoub, hackmancoltaire
 */

#pragma once

#include "utility/AppleMidi_Settings.h"
#include "utility/AppleMidi_Defs.h"

#include "utility/RtpMidi.h"

#include "utility/RtpMidi_Clock.h"

#include "utility/dissector.h"

#if defined(ARDUINO)
#if defined(ESP8266)
#define MAX_SESSIONS 4 // arbitrary number (tested up to 4 clients)
#else
#define MAX_SESSIONS 2 // Arduino can open max 4 socket. Each session needs 2 UDP ports. (Each session takes 228 bytes)
#endif
#else
#define MAX_SESSIONS 4 // arbitrary number
#endif

#include "IAppleMidiCallbacks.h"

#include "AbstractMidiInterface.h"
using namespace Midi;

BEGIN_APPLEMIDI_NAMESPACE

/*! \brief The main class for AppleMidiInterface handling.\n
	See member descriptions to know how to use it,
	or check out the examples supplied with the library.
 */
template<class UdpClass, class Settings = DefaultSettings>
class AppleMidiInterface : public AbstractMidiInterface, public IAppleMidiCallbacks
{
protected:
	//
	UdpClass _controlPort;
	UdpClass _dataPort;

	Dissector _controlPortDissector;
	Dissector _dataPortDissector;

	RtpMidi<UdpClass>	_rtpMidi;

	RtpMidi_Clock _rtpMidiClock;

	// SSRC, Synchronization source.
	// (RFC 1889) The source of a stream of RTP packets, identified by a 32-bit numeric SSRC identifier
	// carried in the RTP header so as not to be dependent upon the network address. All packets from a
	// synchronization source form part of the same timing and sequence number space, so a receiver groups
	// packets by synchronization source for playback. Examples of synchronization sources include the
	// sender of a stream of packets derived from a signal source such as a microphone or a camera, or an
	// RTP mixer. A synchronization source may change its data format, e.g., audio encoding, over time.
	// The SSRC identifier is a randomly chosen value meant to be globally unique within a particular RTP
	// session. A participant need not use the same SSRC identifier for all the RTP sessions in a
	// multimedia session; the binding of the SSRC identifiers is provided through RTCP. If a participant
	// generates multiple streams in one RTP session, for example from separate video cameras, each must
	// be identified as a different SSRC.
	uint32_t _ssrc;

	Session_t	Sessions[MAX_SESSIONS];

	char _sessionName[SESSION_NAME_MAX_LEN + 1];

	byte _packetBuffer[PACKET_MAX_SIZE];

	inline uint32_t	createInitiatorToken();

public:
	// Constructor and Destructor
    AppleMidiInterface();
	~AppleMidiInterface();

	int Port;

	bool begin(const char*, uint16_t port = CONTROL_PORT);

	uint32_t	getSynchronizationSource();
	char*	getSessionName() { return _sessionName; }

	inline void run();

	// IAppleMidi

	void invite(IPAddress ip, uint16_t port = CONTROL_PORT);

    // Callbacks
	void OnInvitation(void* sender, AppleMIDI_Invitation&);
	void OnEndSession(void* sender, AppleMIDI_EndSession&);
	void OnReceiverFeedback(void* sender, AppleMIDI_ReceiverFeedback&);

	void OnInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);
	void OnControlInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);
	void OnContentInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);

	void OnSyncronization(void* sender, AppleMIDI_Syncronization&);
	void OnBitrateReceiveLimit(void* sender, AppleMIDI_BitrateReceiveLimit&);
	void OnControlInvitation(void* sender, AppleMIDI_Invitation&);
	void OnContentInvitation(void* sender, AppleMIDI_Invitation&);
    
    // Session mamangement
    int  GetFreeSessionSlot();
    int  GetSessionSlotUsingSSrc(const uint32_t ssrc);
    int  GetSessionSlotUsingInitiatorToken(const uint32_t initiatorToken);
    void CreateLocalSession(const int slot, const uint32_t ssrc);
    void CreateRemoteSession(IPAddress ip, uint16_t port);
    void CompleteLocalSessionControl(AppleMIDI_InvitationAccepted& invitationAccepted);
    void CompleteLocalSessionContent(AppleMIDI_InvitationAccepted& invitationAccepted);
    void DeleteSession(const uint32_t ssrc);
    void DeleteSession(int slot);
    void DeleteSessions();
    
    void DumpSession();
    
    void ManageInvites();
    void ManageTiming();

private:
    void write(UdpClass&, AppleMIDI_InvitationRejected,  IPAddress ip, uint16_t port);
    void write(UdpClass&, AppleMIDI_InvitationAccepted,  IPAddress ip, uint16_t port);
    void write(UdpClass&, AppleMIDI_Syncronization,      IPAddress ip, uint16_t port);
    void write(UdpClass&, AppleMIDI_Invitation,          IPAddress ip, uint16_t port);
    void write(UdpClass&, AppleMIDI_BitrateReceiveLimit, IPAddress ip, uint16_t port);

protected:
    virtual void send(MidiType inType, DataByte inData1, DataByte inData2, Channel){};
    virtual void send(MidiType inType, DataByte inData1, DataByte inData2){};
    virtual void send(MidiType inType, DataByte inData){};
    virtual void send(MidiType inType){};
    virtual void sendSysEx(byte, const byte* inData, byte, uint16_t){};

private:
    void internalSend(Session_t&, MidiType inType, DataByte inData1, DataByte inData2, Channel inChannel);
    void internalSend(Session_t&, MidiType inType, DataByte inData1, DataByte inData2);
    void internalSend(Session_t&, MidiType inType, DataByte inData);
	void internalSend(Session_t&, MidiType inType);
	void internalSendSysEx(Session_t&, byte, const byte*, byte, uint16_t);

public:
	void OnConnected(void(*fptr)(uint32_t, char*));
	void OnDisconnected(void(*fptr)(uint32_t));

private:
    void launchCallback();

	void(*mConnectedCallback)(uint32_t, char*);
	void(*mDisconnectedCallback)(uint32_t);
};

END_APPLEMIDI_NAMESPACE

// -----------------------------------------------------------------------------

#include "utility/packet-rtp-midi.h"
#include "utility/packet-apple-midi.h"

#include "AppleMidiInterface.hpp"
