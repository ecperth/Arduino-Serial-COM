#define WAIT_RTS_ASSERT 1
#define READ_CMD 2
#define WAIT_RTS_CLEARED 3
#define SEND_RESPONSE 4
#define ERROR_STATE 10

//byte cmd[30];     // Buffer for incoming command
String cmd;
char cmdChar[30]; // incoming command in ascii form
int cmdCount;
byte resp[] = {0xAA, 0x0A, 0x09, 0x00, 0x01, 0x8F, 0x62, 0x62, 0xF7, 0x58};//response
int recByteCount;

//int BUTTON_LED_RED = 10;   // set by Controller
//int RTS_LED_YELLOW = 11;   // set by Controller
int CTS_LED_GREEN = 12;    // set by Strigr
int DATA_LED_WHITE = 7;    // for counting command bytes

int RTS = 2;
int CTS = 3;

int state = WAIT_RTS_ASSERT;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(CTS_LED_GREEN, OUTPUT);
  pinMode(DATA_LED_WHITE, OUTPUT);
  pinMode(RTS, INPUT);
  pinMode(CTS, OUTPUT);
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
//Serial.println("Initialising...");
//Serial.flush(); // waits for data to be transmitted
  digitalWrite(CTS,HIGH);           // make sure CTS is not asserted 
  digitalWrite(CTS_LED_GREEN,LOW);  // switch off CTS greenLED
  digitalWrite(DATA_LED_WHITE,LOW); //switch off DATA white LED
}

void loop() {
//Serial.println("GO");
  switch(state){

    case WAIT_RTS_ASSERT:
    cmd="";

    if (digitalRead(RTS)==LOW){ // received RTS
      delay(100);
      if (digitalRead(RTS)==LOW){ 

        digitalWrite(CTS,LOW);             // assert CTC
        digitalWrite(CTS_LED_GREEN,HIGH);  // switch on CTS green LED

        state = READ_CMD;   // now wait for the command from the controller
      }
    }  
    break;

    case READ_CMD:
    //Serial.println("WAIT_FOR_CMD");
    delay(100);
    if (Serial.available()==0){ break;}
    Serial.find('A');
    Serial.find('A');
    cmd = Serial.readStringUntil('Z');
    for (int n=0; n<cmd.length(); n++){// one flash for each byte recd
       digitalWrite(DATA_LED_WHITE,HIGH);  // switch on DATA white LED
       delay(200);
       digitalWrite(DATA_LED_WHITE,LOW);  // switch off DATA white LED
       delay(200);
    }
    state = WAIT_RTS_CLEARED;   
    break;
  
    case WAIT_RTS_CLEARED:
    //Serial.println("WAIT_RTS_HIGH");
      if (digitalRead(RTS)==HIGH){
        delay(100);
        if (digitalRead(RTS)==HIGH){
          delay(100);
          digitalWrite(CTS,HIGH);             // clear CTC
          digitalWrite(CTS_LED_GREEN,LOW);    // switch off CTS green LED
          state = SEND_RESPONSE;
        }
      }
      break;

    case SEND_RESPONSE:
    //Serial.println("WAIT_RTS_HIGH");
    delay(100);
    Serial.print("AA00000000Z"); //Write the cmd to String
    Serial.flush();
    state = WAIT_RTS_ASSERT;

    break;

    case ERROR_STATE:   // not used
    break;

    default:
      //Serial.println("No State"); 
      //Serial.flush();
    break;
  } // end case
} // end loop

void databyte_to_hex_char(byte b, int x){
  
  byte hi_nib = (byte) (b & 0x0F);
  byte lo_nib = (byte)((b & 0xF0) >> 4);
  
  if (hi_nib < 0xA){
    hi_nib += 0x30;
  }
  else{
    hi_nib += 0x41 - 0xA;
  }

  if (lo_nib < 0xA){
  	lo_nib += 0x30;
  }
  else{
    lo_nib += 0x41 - 0xA;
  }
  cmdChar[x+1] = hi_nib;
  cmdChar[x] = lo_nib;
}

