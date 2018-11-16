#define WAIT_BUTTON 1
#define WAIT_CTS_ASSERT 2
#define SEND_CMD 3
#define WAIT_CTS_CLEAR 4
#define READ_RESPONSE 5
#define ERROR_STATE 10

byte cmd[] = {0xAA, 0x0A, 0x30, 0x00, 0x0A, 0xAA, 0xBB, 0xCC, 0x90, 0x84}; //String data
//char resp[30]; // buffer for incoming response
String resp;
int respCount; // number of characters read

int BUTTON = 9;

int BUTTON_LED_RED = 10;
int RTS_LED_YELLOW = 11;   // set by Controller
//int CTS_LED_GREEN = 12;  // set by Stingr
int DATA_LED_WHITE = 7;    // for counting response bytes

int RTS = 2;
int CTS = 3;

int state = WAIT_BUTTON;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(BUTTON_LED_RED, OUTPUT);
  pinMode(RTS_LED_YELLOW, OUTPUT);
  pinMode(DATA_LED_WHITE, OUTPUT);
  pinMode(BUTTON,INPUT);
  pinMode(RTS, OUTPUT);
  pinMode(CTS, INPUT);
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
//Serial.println("Initialising...");
//Serial.flush(); // waits for data to be transmitted
  digitalWrite(RTS,HIGH);            // make sure RTS is not asserted 
  digitalWrite(RTS_LED_YELLOW,LOW);  // switch off RTS yellow LED
  digitalWrite(BUTTON_LED_RED,HIGH);  // switch on BUTTON red LED
  digitalWrite(DATA_LED_WHITE,LOW);   //switch off DATA white LED
}

void loop() {
//Serial.println("GO");
//Serial.flush(); // waits for data to be transmitted
  switch(state){
    case WAIT_BUTTON:
    resp = "";
    //Serial.println("GO");

      if (digitalRead(BUTTON)==LOW){  // switch is pressed (easy debounce for now)
        delay(100); // wait and see if still low.
        if (digitalRead(BUTTON)==LOW){ // ie if still low

          digitalWrite(RTS,LOW);              //assert RTS
          digitalWrite(RTS_LED_YELLOW,HIGH);  // switch on RTS yellow led
          digitalWrite(BUTTON_LED_RED,LOW);  // switch off BUTTON red LED
          
          state = WAIT_CTS_ASSERT;   // now wait for the CTS low from the Stingr
        } 
      } 
      break;

    case WAIT_CTS_ASSERT:
    //Serial.println("WAIT_FOR_CTS");
      if (digitalRead(CTS)==LOW){  // Stingr has issued CTS (easy debounce for now)
          delay(100);
        if (digitalRead(CTS)==LOW){ 
          state = SEND_CMD;   // now go to SEND_CMD
        }  else { /* continue to loop for now */ }
      }
      break;

    case SEND_CMD :
      //Serial.println("SEND_CMD");
      delay(100);
      Serial.print("AA0000Z"); //Write the cmd to String 
      Serial.flush();     // waits for data to be transmitted
      digitalWrite(RTS,HIGH);             //clear RTS
      digitalWrite(RTS_LED_YELLOW,LOW);   // switch off the RTS yellow LED
      state = READ_RESPONSE;
      break;

    case WAIT_CTS_CLEAR:    // Stringr clears CTS before sending Response
    //Serial.println("WAIT_FOR_CTS");
      if (digitalRead(CTS)==HIGH){  // Stingr has cleared CTS)
        delay(100);
        if (digitalRead(CTS)==HIGH){ 
          state = READ_RESPONSE;   // now go to SEND_CMD
        }  else { /* continue to loop for now */ }
      }
      break;

    case READ_RESPONSE:
      //Serial.println("WAIT_RESP");
      delay(100);  // allow time for Stingr to send data
      if (Serial.available()==0){ break;}
      Serial.find('A');
      Serial.find('A');
      resp = Serial.readStringUntil('Z');  
      for (int n=0; n<resp.length(); n++){  // one flash for each byte recd
         digitalWrite(DATA_LED_WHITE,HIGH);  // switch on DATA white LED
         delay(200);
         digitalWrite(DATA_LED_WHITE,LOW);  // switch off DATA white LED
         delay(200);
      }   
      digitalWrite(BUTTON_LED_RED,HIGH);  // switch on BUTTON red LED
      state = WAIT_BUTTON;  // restart the process
      break;

    case ERROR_STATE:  // not used
      break;

    default:
  //Serial.println("No State"); 
  //Serial.flush();
      break;
  }
}