as /*
-------- CONTROLE DE ACESSO VIA RFID ------------
=================================================
============  Jocimar Roberto Silva  ============
================  Versao Final  =================
=================  23/11/2017  ==================
=================================================
*/
 
// Bibliotecas para utilização do RFID
#include <MFRC522.h>  //  Biblioteca - Dados (Shield RFID)
// #include <SPI.h>  // Biblioteca - Interface (Shield RFID)

// Bibliotecas para utilizacAo do Micro SDCard
#include <SPI.h>  // Biblioteca - Interface (Micro SDCard)
#include <SD.h>

// Bibliotecas de data e hora do modulo RTC DS-3231 conectado via bibliotecas I2C, Wire e RTClib
#include <Wire.h>  // Biblioteca para envio e recebimento de dados I2C
#include <RTClib.h>

// Biblioteca para utilizacAo do Display LCD
#include <LiquidCrystal.h>  // Carrega a Biblioteca LiquidCrystal


// Pinagem Arduino Mega
#define LED_VERMELHO 30 // Pino do Led Vermelho
#define LED_VERDE 31 // Pino do Led Verde
#define BUZZER 2 // Pino (+) do Buzzer
#define SS_PIN 53 // Pino SDA do RFID
#define RST_PIN 48 // Pino Reset do RFID
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // Pinos 8, 9, 4, 5, 6, 7  no Arduino Mega para o LCD

// Variaveis e Constantes
String IDtag = ""; //Variável que armazenará o ID da Tag
bool Permitido = false; //Variável que verifica a permissão
String tagPorta = "";
char st[20];  // Vetor do LCD
int p_rele1 = 22;  // Porta ligada ao pino IN1 do Modulo Rele
char opcao = "";
int botao = 0; // Variavel que armazena o valor lido na porta digital do Push Button
const int SDCARD_SS_PIN = 3;  // Constante que determina o pino CS para 3
const int pushButton = 28; // Pino digital utilizado pelo Push Button

// (RTC_DS3231) Classe de comunicacao que contem as informacores de data / hora
RTC_DS3231 rtc;

// Objeto que manipula os dados do arquvo de Log
File myFile;
 
// Vetor responsável por armazenar os ID's das Tag's cadastradas
String TagsCadastradas[] = {"45a3b14f", "Nova_Tag"}; // Chaveiro Cadastrado - ID 45a3b14f

// Cria uma nova instância para o leitor e passa os pinos como parâmetro
MFRC522 LeitorRFID(SS_PIN, RST_PIN);


// ######################################  SETUP  ######################################
 
void setup() 
{
        Serial.begin(9600);                 // Inicializa a comunicação Serial
        SPI.begin();                        // Inicializa comunicacao SPI 
        lcd.begin(16, 2);                   // Definição de colunas e linhas do LCD.        
        LeitorRFID.PCD_Init();              // Inicializa o leitor RFID
        pinMode(LED_VERDE, OUTPUT);         // Declara o pino do led verde como saída
        pinMode(LED_VERMELHO, OUTPUT);      // Declara o pino do led vermelho como saída
        pinMode(BUZZER, OUTPUT);            // Declara o pino do buzzer como saída
        pinMode(p_rele1, OUTPUT);           // Definicao do pino do rele 1 como Saida
        digitalWrite(p_rele1, HIGH);        // Desativa o Rele 1
        pinMode(pushButton, INPUT_PULLUP);  // Define a porta como entrada
        // (INPUT_PULLUP) Modo que dispensa a utilização de um resistor para o Push Button
                
  // Funcao para ajustar a data/hora do RTC (eh realizado apenas 1 vez)
  ajustarRTC();

  // Funcao para inicializar o CartaoSD
  inicializarCartaoSD();

  // Define o numero de colunas e linhas do LCD.
  lcd.begin(16, 2);

  // Funcao que exibe a Mensagem Inicial no LCD      
  mensagemInicial();
}


// #######################################  LOOP  #######################################
 
void loop()
{  
  // Chama a funcao responsavel por fazer a leitura das Tags
  Leitura();
  
  // Chama a funcao de acionamento manual da trava Solenoide pelo Push Button
  botaoAbreTranca();

  // Chama a funcao responsavel por enviar os dados via bluetooth para o App Android
  capturaAndroid();
}


// ######################################  FUNCOES  ######################################


// ==========  RFID  ==========

// Funcao que realiza a Leitura das Tags
void Leitura()
{ 
    IDtag = ""; // Inicialmente IDtag deve estar vazia.
    
    // Verifica se existe uma Tag presente
    if ( !LeitorRFID.PICC_IsNewCardPresent() || !LeitorRFID.PICC_ReadCardSerial() )
    {
        delay(50);
        return;
    }
    
    // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag        
    for (byte i = 0; i < LeitorRFID.uid.size; i++)
    {        
        IDtag.concat(String(LeitorRFID.uid.uidByte[i], HEX));
    }        
    
    // Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
    for (int i = 0; i < (sizeof(TagsCadastradas)/sizeof(String)); i++)
    {
      if(  IDtag.equalsIgnoreCase(TagsCadastradas[i])  )
      {
          Permitido = true; // Variável Permitido assume valor verdadeiro caso o ID Lido esteja cadastrado
      }
    }       
    if(Permitido == true) 
    {
      acessoLiberado(); //Se a variável Permitido for verdadeira será chamada a função acessoLiberado()        
    }
    else 
    {
      acessoNegado(); //Se não será chamada a função acessoNegado() 
    }
    delay(2000); // aguarda 2 segundos para efetuar uma nova leitura
}


// ==========  LCD  ==========

// Funcao que imprime os textos de permissao no Display LCD
void lcdMsgPermitido()
{
  lcd.clear();  // Limpa a tela do Display LCD
  lcd.setCursor(0, 0);  // Posiciona o cursor na posição 0 da primeira linha do Display LCD
  lcd.print("  OLA USUARIO   ");
  lcd.setCursor(0, 1);  // Posiciona o cursor na posição 0 da segunda linha do Display LCD
  lcd.print( "ACESSO PERMITIDO" );
  delay(3000);  // Espera de 3 seg
  lcd.clear();  // Limpa a tela do Display LCD
  mensagemInicial(); // Exibe a mensagem inicial no Display LCD
}

// Funcao que imprime os textos de negado no Display LCD
void lcdMsgNegado ()
{
  lcd.clear();  // Limpa a tela do Display LCD
  lcd.setCursor(0, 0);  // Posiciona o cursor na posição 0 da primeira linha do Display LCD
  lcd.print( "  CARTAO  " );
  lcd.setCursor(0, 1);  // Posiciona o cursor na posição 0 da segunda linha do Display LCD
  lcd.print( "  DESCONHECIDO  " );
  delay(2000);  // Espera de 3 seg
  lcd.clear();  // Limpa a tela do Display LCD
  lcd.setCursor(0, 0);  // Posiciona o cursor na posição 0 da primeira linha do Display LCD
  lcd.print( " ACESSO NEGADO  " );
  lcd.setCursor(0, 1);  // Posiciona o cursor na posição 0 da segunda linha do Display LCD
  lcd.print( "CONTATE O ADMIN." );
  delay(3000);  // Espera de 3 seg
  lcd.clear();
  mensagemInicial(); // Exibe a mensagem inicial no Display LCD
}

// Funcao que imprime a mensagem inicial no Display LCD
void mensagemInicial()
{
  lcd.clear();
  lcd.print( "   APROXIME O   " );
  lcd.setCursor(0, 1);   // Posiciona o cursor na posição 0 da segunda linha do Display LCD
  lcd.print( "CARTAO NO LEITOR" );
}


// ==========  LEDS E BUZZER  ==========

// Funcao que emite 2 Bips dando Permissao via Buzzer e acende o LED Verde (2 vezes).
void efeitoPermitido()
{  
  int qtd_bips = 2;  // Define a quantidade de Bips.
  for(int j=0; j<qtd_bips; j++)
  {
    tone(BUZZER,1500);  // Liga o Buzzer com uma frequencia de (1500hz) e liga o LED verde.
    digitalWrite(LED_VERDE, HIGH);
    delay(100);
    
    // Desliga o Buzzer e LED verde.      
    noTone(BUZZER);
    digitalWrite(LED_VERDE, LOW);
    delay(100);
  }
  acionarSolenoide(); // Chama a funcao que aciona a Solenoide. 
}

// Funcao que emite 1 Bip negando Permissao via Buzzer e acende o Led Vermelho (1 vez).
void efeitoNegado()
{  
  int qtd_bips = 1;  // definE a quantidade de Bips.
  for(int j=0; j<qtd_bips; j++)
  {   
    tone(BUZZER,500);  // Liga o Buzzer com uma frequencia de 500hz e liga o LED vermelho.
    digitalWrite(LED_VERMELHO, HIGH);   
    delay(500); 

    noTone(BUZZER);  // Desliga o Buzzer e o LED vermelho.
    digitalWrite(LED_VERMELHO, LOW);
    delay(500);
  }  
}


// ==========  Leitor de cartão microSD  ==========

// Funcao que inicializa o Cartao MicroSD
void inicializarCartaoSD()
{
   if (!SD.begin(SDCARD_SS_PIN)) 
 {
    Serial.println("Falha de inicializacao do CartaoSD!");
    return;
  }
  // Serial.println("Inicializacao do CartaoSD realizada com Sucesso!");

  /* Abra o arquivo. Note que apenas um arquivo pode ser aberto por vez,
   * então você precisa fechar esse arquivo antes de abrir outro.
  */  
}

/*
* Funcao que escreve uma linha no formato: 
* dd/mm/aaaa hh:mm:ss, <cod da Tag> P<numero>, Permitido ou Negado #  
*/
void escreverCartaoSD()
{
  myFile = SD.open("log.txt", FILE_WRITE);
  if (myFile)  // Se o arquivo for aberto, escreva para ele:
  {
    DateTime now = rtc.now();  // Obtem a data e hora correntes do RTC e armazena em now
    
    // Imprime a data no formato ( dd/mm/aaaa )
    if (now.day() < 10) // Adiciona um 0 caso o valor seja menor que 10
    {
      myFile.print("0");
    }
    myFile.print(now.day(), DEC);
    myFile.print('/');
    if (now.month() < 10) // Adiciona um 0 caso o valor seja menor que 10
    {
      myFile.print("0");
    }
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.year(), DEC);
    myFile.print(' ');
    
    // Imprime a hora no formato ( hh:mm:ss )
    if (now.hour() < 10) // Adiciona um 0 caso o valor seja menor que 10
    {
      myFile.print("0");
    }
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    if (now.minute() < 10) // Adiciona um 0 caso o valor seja menor que 10
    {
      myFile.print("0");
    }
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    if (now.second() < 10) // Adiciona um 0 caso o valor seja menor que 10
    {
      myFile.print("0");
    }
    myFile.print(now.second(), DEC);
    myFile.print(", ");

    // Funcao que imprime o cod da Tag RFID + Porta Acessada. Ex:- ( 1A2B3C P01 )
    portaAcesso();
    myFile.print(IDtag + tagPorta); 
    myFile.print(", ");

    escreverPermissao();  // Escreve o texto "Permitido" ou "Negado" conforme o resultado obtido
   
    myFile.println(" #");  // Imprime o caracter de checagem para quebra de linha no Android
    myFile.close();  // Fechando o arquivo:

    // Serial.println("arquivo atualizado com sucesso!");
  }
  else 
  {
    Serial.println("ERRO ao abrir log.txt");  // Se o arquivo não foi aberto, imprima um erro:
  }  
}

// Funcao que escreve "Permitido" caso o acesso seja liberado ou "Negado" se o acesso for negado
void escreverPermissao()
{
    if(Permitido == 1)
    {
      myFile.print("Permitido");  // Imprime o Texto Permitido
    }
    else
    {
    myFile.print("Negado");  // Imprime o Texto Negado
    }
}

// Funcao que vincula a Tag a uma Porta
void portaAcesso()
{
  if(IDtag == "45a3b14f")
  {
    tagPorta = " P01";
  }
  else if(IDtag == "Nova_tag")
  {
    tagPorta = " P02";  
  }
  else
  {
    tagPorta = "  xxx";
  }
}


// ==========  RTC DS3231  ==========

// Funcao que ajusta a Data/Hora caso o RTC fique sem energia (sem bateria)
void ajustarRTC()
{
  if (! rtc.begin()) 
  {
    Serial.println("Nao foi possivel encontrar o RTC");
    while (1);
  }
  if (rtc.lostPower())
  {
    Serial.println("Caso o RTC for desligado, redefine o horario!");
    
    // A linha a seguir define o RTC na data e hora em que esse Sketch foi compilado.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    /* 
     * A funcao acima ajusta o RTC com uma data e hora explícitas, por exemplo:
     * para definir a data --> Novembro 10, 2017 as 3 horas da manha seria feita a chamada:
     * rtc.adjust(DateTime(2017, 11, 10, 3, 0, 0));
    */
  }
}


// ==========  SOLENOIDE  ==========

// Funcao que realiza o acionamento da Trava Solenoide
void acionarSolenoide()
{
  digitalWrite(p_rele1, LOW);  //  Ativa o Rele 1 / Destrava a Solenoide
  delay(3000);
  digitalWrite(p_rele1, HIGH);  // Desativa o Rele 1 / Trava a Solenoide 
}


// ==========  PUSH BUTTON  ==========

// Funcao que libera a porta ativando a trava Solenoide manualmente pressionando o Push Button
void botaoAbreTranca()
{
  // Le o valor na porta digital e armazena na variavel
  botao = digitalRead(pushButton);
  if(botao == 0) // Se o valor lido for igua a 0:
  { 
    acionarSolenoide();
  }
}


// ==========  Monitor Serial  ==========

// Funcao que imprime o conteudo do arquivo no Monitor Serial
void reabrirArquivo()
{
  myFile = SD.open("log.txt");  // Reabrir o arquivo para leitura
  if (myFile) 
  {
    while (myFile.available())  // Leia todo o conteudo do arquivo 
    {
      Serial.write(myFile.read());
    }
    myFile.close();  // Fecha o arquivo
  } 
  else
  {
    Serial.println("ERRO ao reabrir log.txt");  // Se o arquivo não foi aberto, imprima um erro
  }    
}


// ==========  LOOP (FUNCOES AGRUPADAS)  ==========

// Funcao que libera o acesso 
void acessoLiberado()
{
  efeitoPermitido();  // Funcao que Acende o Led Verde e aciona o Buzzer para Permitido

  lcdMsgPermitido(); // Funcao que exibe no Display LCD as Mensagens de Permissao de Acesso
  
  escreverCartaoSD(); // Funcao para acessar , abrir, escrever e fechar o arquivo log.txt do CartaoSD

  reabrirArquivo(); // Funcao abre e le o arquivo log.txt e imprime no Monitor Serial   
     
  Permitido = false;  // Seta a variável Permitido como false novamente
}

// Funcao que nega o acesso
void acessoNegado()
{
  efeitoNegado();  // Funcao que Acende o Led VErmelho e aciona o Buzzer para Negado

  lcdMsgNegado(); // Funcao que exibe no Display LCD as Mensagens de Negacao de Acesso
 
  escreverCartaoSD(); // Funcao para acessar , abrir, escrever e fechar o arquivo log.txt do CartaoSD

  reabrirArquivo(); // Funcao que abre e le o arquivo log.txt e imprime no Monitor Serial  
}


// ==========  APP - ANDROID  ==========

// Funcao que chama a captura de dados via "Serial" do App Android quando conectado
void capturaAndroid()
{
  while(Serial.available() > 0) 
  {
    char opcao = Serial.read();
    if(opcao!='\n') 
    {
      switch(opcao) 
      {
        case '0':
          myFile = SD.open("log.txt");  // Reabrir o arquivo para leitura
          if (myFile) 
          {
            while (myFile.available())  // Leia todo o conteudo do arquivo 
            {
              Serial.write(myFile.read());
            }
            myFile.close();  // Fecha o arquivo
          } 
          else
          {
            Serial.println("ERRO ao reabrir log.txt");  // Se o arquivo não foi aberto, imprima um erro
          }
          break;    
        default:
          Serial.print("Sem leitura");
          break;
      }
    }
  }   
}


