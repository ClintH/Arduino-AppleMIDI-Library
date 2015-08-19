
#include <SPI.h>
#include <Ethernet.h>

#include "AppleMidi.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

unsigned long t0 = millis();
bool isConnected = false;

APPLEMIDI_CREATE_DEFAULT_INSTANCE(); // see definition in AppleMidi_Defs.h

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  // Serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("Getting IP address...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println();
    Serial.println( "Failed DHCP, check network cable & reboot" );
    for (;;)
      ;
  }

  Serial.println();
  Serial.print("IP address is ");
  Serial.println(Ethernet.localIP());

  Serial.println("OK, now make sure you an rtpMIDI session that is Enabled");
  Serial.print("Add device named Arduino with Host/Port ");
  Serial.print(Ethernet.localIP());
  Serial.println(":5004");
  Serial.println("Then press the Connect button");
  Serial.println("Then open a MIDI listener (eg MIDI-OX) and monitor incoming notes");

  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin("test");

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);

  Serial.println("Sending NoteOn/Off of note 45, every second");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  // Listen to incoming notes
  AppleMIDI.run();

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)
  if (isConnected && (millis() - t0) > 1000)
  {
    t0 = millis();
    //   Serial.print(".");

    int note = 45;
    int velocity = 55;
    int channel = 1;

    AppleMIDI.noteOn(note, velocity, channel);
    AppleMIDI.noteOff(note, velocity, channel);
  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(char* name) {
  isConnected = true;
  //  Serial.print("Connected to session ");
  //  Serial.println(name);
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected() {
  isConnected = false;
  //  Serial.println("Disconnected");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOn from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOff from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
}