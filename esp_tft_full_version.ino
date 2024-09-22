
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>


#include "elem/XSlider.h"
#include "elem/XTextbox.h"
#include "elem/XKeyPad_Num.h"
#include "elem/XTogglebtn.h"


String URL = "http://192.168.1.33/data_control/led.php";
String URL_servo = "http://192.168.1.33/data_control/servo.php";


const char* ssid = ""; 
const char* password = ""; 

enum {E_PG_PASSWORD,E_POP_KEYPAD,E_PG_MAIN,E_PG_EXTRA,E_PG_EXTRA2,E_PG_EXTRA3};
enum {E_BTN_TOGGLE,E_TXT_VAL1,E_ELEM_KEYPAD,E_TXT_PASS_CONTROL,
E_SLIDER,E_ELEM_BTN_LOGIN,E_ELEM_BTN_QUIT,E_ELEM_TXT_TEMP,E_ELEM_TXT_HUM,E_ELEM_BTN_NEXT,E_ELEM_BTN_ON_LED,E_ELEM_BTN_LED_OFF,
E_ELEM_BTN_MAIN, E_ELEM_BTN_EXTRA,E_ELEM_BTN_BACK,E_ELEM_BTN_EXTRA2,E_ELEM_BTN_EXTRA3,E_ELEM_BTN_TEMP_UPDATE,E_ELEM_BTN_ANGLE_UPDATE};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_TXT_SIGNATURE,E_FONT_TXT_BUT,E_FONT_TITLE,MAX_FONT}; 

bool      m_bQuit = false;

#define TBOX_ROWS           12  
#define TBOX_COLS           16  


#define MAX_PAGE                6


#define MAX_ELEM_PG_PASSWORD       9 
#define MAX_ELEM_PG_PASSWORD_RAM    MAX_ELEM_PG_PASSWORD  

#define MAX_ELEM_PG_MAIN        9
#define MAX_ELEM_PG_MAIN_RAM    MAX_ELEM_PG_MAIN  

#define MAX_ELEM_PG_EXTRA       20  
#define MAX_ELEM_PG_EXTRA_RAM   MAX_ELEM_PG_EXTRA 

#define MAX_ELEM_PG_EXTRA2       20               
#define MAX_ELEM_PG_EXTRA2_RAM  MAX_ELEM_PG_EXTRA2 

#define MAX_ELEM_PG_EXTRA3       20                
#define MAX_ELEM_PG_EXTRA3_RAM  MAX_ELEM_PG_EXTRA3 

#define MAX_ELEM_POP_KEYPAD 1
#define MAX_ELEM_POP_KEYPAD_RAM MAX_ELEM_POP_KEYPAD

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];

gslc_tsElem                 m_asPASSWORDElem[MAX_ELEM_PG_PASSWORD_RAM];
gslc_tsElemRef              m_asPASSWORDElemRef[MAX_ELEM_PG_PASSWORD];

gslc_tsElem                 m_asMainElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asMainElemRef[MAX_ELEM_PG_MAIN];

gslc_tsElem                 m_asExtraElem[MAX_ELEM_PG_EXTRA_RAM];
gslc_tsElemRef              m_asExtraElemRef[MAX_ELEM_PG_EXTRA];

gslc_tsElem                 m_asExtra2Elem[MAX_ELEM_PG_EXTRA2_RAM];
gslc_tsElemRef              m_asExtra2ElemRef[MAX_ELEM_PG_EXTRA2];

gslc_tsElem                 m_asExtra3Elem[MAX_ELEM_PG_EXTRA3_RAM];
gslc_tsElemRef              m_asExtra3ElemRef[MAX_ELEM_PG_EXTRA3];

gslc_tsElem                 m_asPopKeypadElem[MAX_ELEM_POP_KEYPAD_RAM];
gslc_tsElemRef              m_asPopKeypadElemRef[MAX_ELEM_POP_KEYPAD];

gslc_tsXKeyPad              m_sKeyPadNum; 

gslc_tsXSlider              m_sXSlider;
gslc_tsXSlider              m_sXSliderText;

gslc_tsXTogglebtn               m_sXTogglebtn1;

WiFiClient client;

#define MAX_STR             20

 
gslc_tsElemRef*  m_pElemCnt = NULL;
gslc_tsElemRef*  m_pElemProgress = NULL;

gslc_tsElemRef*  m_pElemVal1 = NULL;
gslc_tsElemRef*  m_pElemKeyPad = NULL;
gslc_tsElemRef*    m_pElemResult = NULL;
  
gslc_tsElemRef*  m_pElemLEDCONTROL    = NULL;


static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }


bool CbBtnCommon(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
    gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
    gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
    gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);
  
    int16_t nElemId = pElem->nId;

    if (eTouch == GSLC_TOUCH_UP_IN) {

      if (nElemId == E_BTN_TOGGLE) {
          
          bool bNewState = gslc_ElemXTogglebtnGetState(pGui, pElemRef);
            
          gslc_ElemXTogglebtnSetState(pGui, pElemRef, bNewState);

          int status = (bNewState) ? 1 : 0;
          connectWiFi();
          sendled(status);
          digitalWrite(2, HIGH);
          delay(50);
          digitalWrite(2, LOW);
          WiFi.disconnect(true);
          
        } 

      else if (nElemId == E_ELEM_BTN_QUIT) {
            
          digitalWrite(2,LOW);
          gslc_SetPageCur(&m_gui,E_PG_PASSWORD);
          gslc_ElemSetTxtStr(pGui, m_pElemResult, "--- WELCOME ---");
       
          m_bQuit = false;
          

            
        } 
      else if (nElemId == E_ELEM_BTN_EXTRA) {
        
        gslc_SetPageCur(pGui, E_PG_EXTRA);
        }

      else if (nElemId == E_ELEM_BTN_EXTRA2) {
        gslc_SetPageCur(pGui, E_PG_EXTRA2);
        
        } 

      else if (nElemId == E_ELEM_BTN_EXTRA3) {
        gslc_SetPageCur(pGui, E_PG_EXTRA3);
        
        }

      else if (nElemId == E_ELEM_BTN_MAIN) {
        gslc_SetPageCur(pGui, E_PG_MAIN);
        
        }
/*
      else if (nElemId == E_ELEM_BTN_BACK) {
           
        gslc_tsPage* pPage = gslc_PageFindById(&m_gui, gslc_GetPageCur(pGui));
          if (pPage != NULL && pPage->nPageId > 0) {
              gslc_SetPageCur(pGui, pPage->nPageId - 1);
            }
        }
        else if (nElemId == E_ELEM_BTN_NEXT) {
            // Navigate to the next page
            gslc_tsPage* pPage = gslc_PageFindById(&m_gui, gslc_GetPageCur(pGui));
            if (pPage != NULL && pPage->nPageId < MAX_PAGE - 1) {
                gslc_SetPageCur(pGui, pPage->nPageId + 1);
            }
        }
        /*
        else if (nElemId == E_ELEM_BTN_ON_LED) {
          int status = 1;
          connectWiFi();

          sendled(status);
          digitalWrite(2, HIGH);

          for (int led = 1; led <= 5; led++)
          {
            digitalWrite(2, HIGH);
            delay(100);
            digitalWrite(2, LOW);
            delay(100);

          }

          delay(1000);
          WiFi.disconnect(true);
          
            
        }

        /*
        else if (nElemId == E_ELEM_BTN_LED_OFF) {
          int status = 0;
          connectWiFi();
          
          sendled(status);
          
          for (int led = 1; led <= 5; led++)
          {
            digitalWrite(2, HIGH);
            delay(100);
            digitalWrite(2, LOW);
            delay(100);
          }
          delay(1000);
          WiFi.disconnect(true);
         
         
            
        }
*/
        else if (nElemId == E_ELEM_BTN_TEMP_UPDATE) {
          

          connectWiFi();

          readtemp();
          digitalWrite(2, HIGH);
          delay(100);
          digitalWrite(2, LOW);
          WiFi.disconnect(true);
         
         
            
        }
        else if (nElemId == E_ELEM_BTN_ANGLE_UPDATE) {
          
          connectWiFi();
          send_angle();
          digitalWrite(2, HIGH);
          delay(100);
          digitalWrite(2, LOW);
          WiFi.disconnect(true);
         
         
            
        }
    }
    return true;
}


void readtemp(){
const char* host = "http://192.168.1.33/";
  HTTPClient http;
  String getAddress = "GET_DATA/GetData.php"; 
  String linkGet = host + getAddress; 

  http.begin(linkGet);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCodeGet = http.POST("ID=0");
  String payloadGet = http.getString();
  Serial.println(payloadGet);

  int commaIndex = payloadGet.indexOf(',');
String temperatureStr = payloadGet.substring(0, commaIndex);
String humidityStr = payloadGet.substring(commaIndex + 1);


gslc_tsElemRef* pElemTemp = gslc_PageFindElemById(&m_gui, E_PG_EXTRA3, E_ELEM_TXT_TEMP);

gslc_tsElemRef* pElemHum = gslc_PageFindElemById(&m_gui, E_PG_EXTRA3, E_ELEM_TXT_HUM);

    if (pElemTemp && pElemHum ) {

    
        float temperature = temperatureStr.toFloat();
        float humidity = humidityStr.toFloat();

        char strTemp[40];
        //snprintf(strTemp, sizeof(strTemp), "%.2f C", ""); 
        snprintf(strTemp, sizeof(strTemp), "%.2f C", temperature); 
        
        gslc_ElemSetTxtStr(&m_gui, pElemTemp, strTemp);

        char strHum[60];
        //snprintf(strHum, sizeof(strHum), "%.2f %", ""); 
        snprintf(strHum, sizeof(strHum), "%.2f %%", humidity); 
        
        gslc_ElemSetTxtStr(&m_gui, pElemHum, strHum);

      
    }

}
void sendled(int status)
{


   String postData = "Stat=" + String(status); 

    HTTPClient http; 
    http.begin(URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    
    int httpCode = http.POST(postData); 
    String payload = ""; // payload değişkeni boş bir string olarak başlatılır
    
    if(httpCode > 0) {
      // file found at server
      if(httpCode == HTTP_CODE_OK) {
        payload = http.getString(); // payload değişkeni HTTP isteği sonrası gelen verilerle güncellenir
        Serial.println(payload);
      } else {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();  
    
    Serial.println("------------");


}

bool CbBtnCommonPASS(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(&m_gui,pElemRef);
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;

  char acTxtNum[11];
  int32_t nVal1;

  if (eTouch == GSLC_TOUCH_UP_IN) {
    
    switch (pElem->nId) {
     
    case E_TXT_VAL1:
      gslc_ElemXKeyPadInputAsk(pGui, m_pElemKeyPad, E_POP_KEYPAD, m_pElemVal1);
      break;
    }
  }
  return true;
  
}

// KeyPad Input Ready callback
bool CbInputCommon(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui,pElemRef);

  // From the element's ID we can determine which element is ready.
  if (pElem->nId == E_ELEM_KEYPAD) {
    int16_t nTargetElemId = gslc_ElemXKeyPadDataTargetIdGet(pGui, pvData);
    char acTxtNum[11];
    int32_t nVal1;
    switch (nState) {
    case XKEYPAD_CB_STATE_DONE:

      if (nTargetElemId == E_TXT_VAL1) {
      gslc_ElemSetTxtStr(pGui, m_pElemVal1, gslc_ElemXKeyPadDataValGet(pGui, pvData));
      nVal1 = atol(gslc_ElemGetTxtStr(pGui, m_pElemVal1));
      ltoa(nVal1, acTxtNum, 10);
      
      gslc_PopupHide(pGui);
      char* new_pass = gslc_ElemXKeyPadDataValGet(pGui, pvData);
      
      if(strcmp(new_pass, "0000") == 0) {


          gslc_SetPageCur(pGui, E_PG_MAIN);
          gslc_ElemSetTxtStr(pGui, m_pElemVal1, "");
          

      } else {
          
          gslc_ElemSetTxtStr(pGui, m_pElemVal1, "");
          gslc_ElemSetTxtStr(pGui, m_pElemResult, "INCORRECT PASSWORD!!!");

      }

      }

      break;

    case XKEYPAD_CB_STATE_CANCEL:
      

      gslc_PopupHide(pGui);
      break;
    }
  }
  return true;
}
int nAngle = 0;

bool CbControls(void* pvGui, void* pvElemRef, int16_t nPos)
{
    gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
    gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
    gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui, pElemRef);

   
    switch (pElem->nId) {
        case E_SLIDER: {
            
            int nVal = gslc_ElemXSliderGetPos(pGui, pElemRef);

            nAngle = map(nVal, 0, 100, 0, 180);

            char acTxt[20];
            snprintf(acTxt, sizeof(acTxt), "%u", nAngle);
            gslc_tsElemRef* pElemRefTmp = gslc_PageFindElemById(pGui, E_PG_EXTRA, E_ELEM_TXT_TEMP);
            gslc_ElemSetTxtStr(pGui, pElemRefTmp, acTxt);

           
            

            break;
        }

    }
    return true;
}

/*
bool CbBtnCommonTOGGLE(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = pElemRef->pElem;
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      case E_BTN_TOGGLE:
        if (gslc_ElemXTogglebtnGetState(pGui, m_pElemShowApp))
          digitalWrite(2,HIGH);

        else 
          digitalWrite(2,LOW);
        break;

      default:
        break;
    } // switch
  }
  return true;
}
*/
void send_angle()
{


   String postData = "Servo=" + String(nAngle); 

    HTTPClient http; 
    http.begin(URL_servo);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    
    int httpCode = http.POST(postData); 
    String payload = ""; // payload değişkeni boş bir string olarak başlatılır
    
    if(httpCode > 0) {
      // file found at server
      if(httpCode == HTTP_CODE_OK) {
        payload = http.getString(); // payload değişkeni HTTP isteği sonrası gelen verilerle güncellenir
        Serial.println(payload);
      } else {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();  
    

    Serial.print("Data: "); Serial.println(postData); 

    Serial.println("-------------------");


}


// Create the default elements on each page
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;
  gslc_PageAdd(&m_gui,E_PG_PASSWORD,m_asPASSWORDElem,MAX_ELEM_PG_PASSWORD_RAM,m_asPASSWORDElemRef,MAX_ELEM_PG_PASSWORD);
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asMainElem,MAX_ELEM_PG_MAIN_RAM,m_asMainElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asExtraElem,MAX_ELEM_PG_EXTRA_RAM,m_asExtraElemRef,MAX_ELEM_PG_EXTRA);
  gslc_PageAdd(&m_gui,E_PG_EXTRA2,m_asExtra2Elem,MAX_ELEM_PG_EXTRA2_RAM,m_asExtra2ElemRef,MAX_ELEM_PG_EXTRA2);
  gslc_PageAdd(&m_gui,E_PG_EXTRA3,m_asExtra3Elem,MAX_ELEM_PG_EXTRA3_RAM,m_asExtra3ElemRef,MAX_ELEM_PG_EXTRA3);
  gslc_PageAdd(&m_gui, E_POP_KEYPAD, m_asPopKeypadElem, MAX_ELEM_POP_KEYPAD_RAM, m_asPopKeypadElemRef, MAX_ELEM_POP_KEYPAD);

  // PAGE: LOGIN
   pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_PASSWORD,(gslc_tsRect){10,10,470,310});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_PASSWORD,(gslc_tsRect){10,10,460,50},
    (char*)"LOGIN PAGE",0,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);

  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_PASSWORD, (gslc_tsRect) { 100,100,100,70},
    (char*)"PASSWORD: ", 0, E_FONT_TXT_BUT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

 static char m_strtxt5[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_VAL1, E_PG_PASSWORD, (gslc_tsRect) { 220, 118, 130, 30 },
    (char*)m_strtxt5, 11, E_FONT_TXT);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK1, GSLC_COL_BLACK, GSLC_COL_BLUE_DK4);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetTxtMargin(&m_gui, pElemRef, 10);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommonPASS);
  m_pElemVal1 = pElemRef;



// ŞİFRE DURUM BİLDİRİ BÖLÜMÜ

  static char m_strResult[30] = "--- WELCOME ---";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TXT_PASS_CONTROL, E_PG_PASSWORD, (gslc_tsRect) { 150, 200, 80, 50 },
    (char*)m_strResult, 30, E_FONT_TXT_BUT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_YELLOW);
  m_pElemResult = pElemRef;

 static gslc_tsXKeyPadCfg_Num sCfg;
  sCfg = gslc_ElemXKeyPadCfgInit_Num();
  gslc_ElemXKeyPadCfgSetFloatEn_Num(&sCfg, true);
  gslc_ElemXKeyPadCfgSetSignEn_Num(&sCfg, true);
 
  m_pElemKeyPad = gslc_ElemXKeyPadCreate_Num(&m_gui, E_ELEM_KEYPAD, E_POP_KEYPAD,
    &m_sKeyPadNum, 100, 80, E_FONT_TXT, &sCfg);
  gslc_ElemXKeyPadValSetCb(&m_gui, m_pElemKeyPad, &CbInputCommon);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_PASSWORD,(gslc_tsRect){320,295,40,20},
    (char*)"Designed by Emin AYYILDIZ",0,E_FONT_TXT_SIGNATURE);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,10,470,310});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create title
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,10,460,50},
    (char*)"MAIN PAGE",3000,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);

    pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){150,150,40,20},
  (char*)"--- WELCOME ---",0,E_FONT_TXT_BUT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){320,295,40,20},
  (char*)"Designed by Emin AYYILDIZ",0,E_FONT_TXT_SIGNATURE);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){438,12,40,38},(char*)"X",200,E_FONT_TITLE,&CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_RED);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);



  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_tsRect){60,230,100,40},(char*)"SERVO",0,E_FONT_BTN,&CbBtnCommon);

    pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA2,E_PG_MAIN,
    (gslc_tsRect){190,230,100,40},(char*)"LED CONTROL",6000,E_FONT_BTN,&CbBtnCommon);
  

       pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA3,E_PG_MAIN,
    (gslc_tsRect){320,230,100,40},(char*)"TEMPERATURE",6000,E_FONT_BTN,&CbBtnCommon);

  // PAGE: SERVO
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){10,10,470,310});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_WHITE);


  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){10,10,460,50},
    (char*)"SERVO CONTROL",300,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);

    pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_EXTRA,
    (gslc_tsRect){438,12,40,38},(char*)"X",200,E_FONT_TITLE,&CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_RED);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){320,295,40,20},
    (char*)"Designed by Emin AYYILDIZ",0,E_FONT_TXT_SIGNATURE);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

    pElemRef = gslc_ElemXSliderCreate(&m_gui, E_SLIDER, E_PG_EXTRA, &m_sXSlider,
                                       (gslc_tsRect){30, 60, 430, 40}, 0, 100, 0, 2, false);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GREEN, GSLC_COL_BLACK, GSLC_COL_BLACK);
    gslc_ElemXSliderSetStyle(&m_gui, pElemRef, true, GSLC_COL_BLUE_DK4, 20, 20, GSLC_COL_RED_DK2);
    gslc_ElemXSliderSetPosFunc(&m_gui, pElemRef, &CbControls);

   static char mstr2[8] = " ";
   pElemRef = gslc_ElemCreateTxt(&m_gui, E_ELEM_TXT_TEMP, E_PG_EXTRA, (gslc_tsRect){220, 110, 100, 40},
                                   mstr2, sizeof(mstr2), E_FONT_TXT_BUT);


  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_ANGLE_UPDATE,E_PG_EXTRA,
    (gslc_tsRect){140,165,200,40},(char*)"UPDATE ANGLE",300,E_FONT_TXT_BUT,&CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_GRAY, GSLC_COL_GRAY);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_YELLOW);
    gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);

  // Create Back button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MAIN,E_PG_EXTRA,
    (gslc_tsRect){60,250,100,40},(char*)"MAIN",300,E_FONT_BTN,&CbBtnCommon);

     pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA2,E_PG_EXTRA,
    (gslc_tsRect){190,250,100,40},(char*)"LED CONTROL",300,E_FONT_BTN,&CbBtnCommon);


    pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA3,E_PG_EXTRA,
    (gslc_tsRect){320,250,100,40},(char*)"TEMPERATURE",300,E_FONT_BTN,&CbBtnCommon);

// PAGE 3
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA2,(gslc_tsRect){10,10,470,310});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK);
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA2,(gslc_tsRect){20,10,460,50},
    (char*)"LED CONTROL",0,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);

      pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_EXTRA2,
    (gslc_tsRect){438,12,40,38},(char*)"X",200,E_FONT_TITLE,&CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_RED);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);


  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA2,(gslc_tsRect){320,295,40,20},
    (char*)"Designed by Emin AYYILDIZ",0,E_FONT_TXT_SIGNATURE);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  // Create Back button with text label

    pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MAIN,E_PG_EXTRA2,
    (gslc_tsRect){60,250,100,40},(char*)"MAIN",300,E_FONT_BTN,&CbBtnCommon);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_EXTRA2,
    (gslc_tsRect){190,250,100,40},(char*)"SERVO",300,E_FONT_BTN,&CbBtnCommon);


  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA3,E_PG_EXTRA2,
    (gslc_tsRect){320,250,100,40},(char*)"TEMPERATURE",300,E_FONT_BTN,&CbBtnCommon);

    
      pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_BTN_TOGGLE,E_PG_EXTRA2,&m_sXTogglebtn1,
    (gslc_tsRect){160,120,150,70},
    GSLC_COL_GRAY,GSLC_COL_GREEN_DK2,GSLC_COL_RED,true, // circular
    false,&CbBtnCommon);

  gslc_ElemSetGlowCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemLEDCONTROL = pElemRef;

/*
    pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_ON_LED, E_PG_EXTRA2,
                                      (gslc_tsRect){160, 150, 70, 40}, (char *) "ON", 0, E_FONT_TXT_BUT, &CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GREEN_DK2, GSLC_COL_GREEN_DK4, GSLC_COL_GREEN_DK1);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);

    pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_LED_OFF, E_PG_EXTRA2,
                                      (gslc_tsRect){250, 150, 70, 40}, (char *) "OFF", 0, E_FONT_TXT_BUT, &CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED_DK2, GSLC_COL_RED_DK4, GSLC_COL_RED_DK1);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);
*/
// PAGE 4

  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA3,(gslc_tsRect){10,10,470,310});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK);
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA3,(gslc_tsRect){10,10,460,50},
    (char*)"TEMPERATURE",300,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);

      pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_EXTRA3,
    (gslc_tsRect){438,12,40,38},(char*)"X",200,E_FONT_TITLE,&CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_RED);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

    pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA3,(gslc_tsRect){320,295,40,20},
    (char*)"Designed by Emin AYYILDIZ",0,E_FONT_TXT_SIGNATURE);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA3,(gslc_tsRect){110,80,40,20},
    (char*)"TEMPERATURE : ",0,E_FONT_TXT_BUT);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE);

 static char mstr1[20] = "..... C";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_TEMP,E_PG_EXTRA3,(gslc_tsRect){270,80,100,20},
    mstr1,sizeof(mstr2),E_FONT_TXT_BUT);
gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA3,(gslc_tsRect){110,130,40,20},
    (char*)"HUMIDITY    : ",0,E_FONT_TXT_BUT);
    gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE);

 static char mstr3[20] = "..... %";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_HUM,E_PG_EXTRA3,(gslc_tsRect){270,130,100,20},
    mstr3,sizeof(mstr2),E_FONT_TXT_BUT);
gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

    pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_TEMP_UPDATE, E_PG_EXTRA3,
                                      (gslc_tsRect){115, 180, 250, 40}, (char *) "UPDATE TEMPERATURE", 0, E_FONT_TXT_BUT, &CbBtnCommon);
    gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED, GSLC_COL_RED, GSLC_COL_RED);
    gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
    gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_MID);

  // Create Back button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA2,E_PG_EXTRA3,
    (gslc_tsRect){190,250,100,40},(char*)"LED CONTROL",300,E_FONT_BTN,&CbBtnCommon);

     pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_EXTRA3,
    (gslc_tsRect){60,250,100,40},(char*)"SERVO",300,E_FONT_BTN,&CbBtnCommon);

      pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MAIN,E_PG_EXTRA3,
    (gslc_tsRect){320,250,100,40},(char*)"MAIN",300,E_FONT_BTN,&CbBtnCommon);

  return true;
}


void setup()
{
  // Initialize debug output
  
  Serial.begin(115200);
    
  gslc_InitDebug(&DebugOut);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
    

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
  if (!gslc_FontSet(&m_gui,E_FONT_TXT_SIGNATURE,GSLC_FONTREF_PTR,NULL,0.5)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_TXT_BUT,GSLC_FONTREF_PTR,NULL,2)) { return; }
  if (!gslc_FontSet(&m_gui,E_FONT_TITLE,GSLC_FONTREF_PTR,NULL,3)) { return; }

 
  InitOverlays();

 
  gslc_SetPageCur(&m_gui,E_PG_PASSWORD);

  m_bQuit = false;
  

}

void loop()
{


gslc_Update(&m_gui);

  
  delay(10);
  
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(50);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
    
  Serial.print("connected to : "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}

