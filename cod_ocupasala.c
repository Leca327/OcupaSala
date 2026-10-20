const int SENSOR_ENTRADA = 2;    // sensor 1 (entrada) 
const int SENSOR_SAIDA   = 3;    // sensor 2 (saída) 
const int LED_VERDE      = 9;    // + do LED verde (linha 43) -> pino 9 
10;   // + do LED vermelho 
const int LED_VERMELHO   = 
(linha 61) -> pino 10 
const int BUZZER         
= 11;   // + do buzzer (linha 11) -> pino 11 
const int LOTACAO_MAXIMA = 5;    // capacidade da sala 
const int DETECTADO      = LOW;  // nível que o sensor envia ao detectar (troque 
para HIGH se o seu for ao contrário) 
const unsigned long TEMPO_PASSAGEM = 2000; // ms que a pessoa tem para 
passar pelo segundo sensor 
// Sons do buzzer 
const int FREQ_ENTRADA = 2500;   // Hz - agudo 
const int FREQ_SAIDA   = 400;    // Hz - grave 
const int FREQ_ALARME  = 1500;   // Hz - alarme de lotação 
const int DURACAO_BIP  = 200;    // ms de cada bip 
const unsigned long INTERVALO_ALARME = 500; // ms entre um bip e outro do 
alarme 
int pessoas = 0;                 
// quantidade atual na sala 
// Estado anterior de cada sensor (para contar só no momento em que é acionado) 
bool entradaAntes = false; 
bool saidaAntes   = false; 
// Bloqueios: ignoram o segundo sensor de uma mesma passagem 
bool ignorarSaida   = false;     // alguém entrou: o sensor de saída não deve contar 
bool ignorarEntrada = false;     // alguém saiu: o sensor de entrada não deve contar 
unsigned long inicioPassagem = 0; 
unsigned long ultimoBip = 0;     // quando o buzzer tocou pela última vez 
void mostrarPessoas(const char* sensor) { 
Serial.print("Ocupação: "); 
Serial.print(sensor); 
 
  Serial.print(" - "); 
  Serial.print(pessoas); 
  Serial.println(" PESSOAS"); 
} 
 
void bip(int frequencia) { 
  tone(BUZZER, frequencia, DURACAO_BIP); // toca e desliga sozinho, sem travar o 
código 
  ultimoBip = millis(); 
} 
 
void setup() { 
  pinMode(SENSOR_ENTRADA, INPUT); 
  pinMode(SENSOR_SAIDA, INPUT); 
  pinMode(LED_VERDE, OUTPUT); 
  pinMode(LED_VERMELHO, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
 
  digitalWrite(LED_VERDE, HIGH);    // verde ligado 
  digitalWrite(LED_VERMELHO, LOW);  // vermelho desligado 
  digitalWrite(BUZZER, LOW);        // buzzer desligado 
 
  Serial.begin(9600); 
} 
 
void loop() { 
  bool entradaAgora = (digitalRead(SENSOR_ENTRADA) == DETECTADO); 
  bool saidaAgora   = (digitalRead(SENSOR_SAIDA) == DETECTADO); 
 
  // Se o tempo da passagem acabou, libera os bloqueios 
  // (cobre o caso de só um sensor ser acionado) 
  if ((ignorarSaida || ignorarEntrada) && millis() - inicioPassagem > 
TEMPO_PASSAGEM) { 
    ignorarSaida = false; 
 
    ignorarEntrada = false; 
  } 
 
  // Sensor de ENTRADA acabou de ser acionado 
  if (entradaAgora && !entradaAntes) { 
    if (ignorarEntrada) { 
      ignorarEntrada = false;       // é o fim de uma saída: não conta 
    } else { 
      pessoas++;                    // alguém está entrando 
      ignorarSaida = true;          // o sensor de saída desta passagem não conta 
      inicioPassagem = millis(); 
      mostrarPessoas("ENTRADA"); 
      bip(FREQ_ENTRADA);            // bip agudo 
    } 
  } 
 
  // Sensor de SAÍDA acabou de ser acionado 
  if (saidaAgora && !saidaAntes) { 
    if (ignorarSaida) { 
      ignorarSaida = false;         // é o fim de uma entrada: não conta 
    } else { 
      if (pessoas > 0) { 
        pessoas--;                  // alguém está saindo (nunca fica negativo) 
      } 
      ignorarEntrada = true;        // o sensor de entrada desta passagem não conta 
      inicioPassagem = millis(); 
      mostrarPessoas("SAÍDA"); 
      bip(FREQ_SAIDA);              // bip grave 
    } 
  } 
 
  entradaAntes = entradaAgora; 
  saidaAntes   = saidaAgora; 
 
 
  // LEDs e alarme conforme a lotação 
  if (pessoas >= LOTACAO_MAXIMA) { 
    digitalWrite(LED_VERDE, LOW);      // sala cheia: verde apaga 
    digitalWrite(LED_VERMELHO, HIGH);  // vermelho acende 
 
    // alarme: bip repetido enquanto estiver lotado 
    if (millis() - ultimoBip >= INTERVALO_ALARME) { 
      bip(FREQ_ALARME); 
    } 
  } else { 
    digitalWrite(LED_VERDE, HIGH);     // tem vaga: verde aceso 
    digitalWrite(LED_VERMELHO, LOW);   // vermelho apagado 
  } 
 
  delay(50); // evita contagens duplicadas por oscilação do sensor 
}