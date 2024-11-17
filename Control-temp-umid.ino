#include <DHT.h>


// Configuração dos pinos e do sensor

#define DHTPIN 7               // Pino conectado ao DHT11

#define DHTTYPE DHT11          // Tipo do sensor utilizado

#define PIN_UMIDIFICADOR 8     // Pino de controle do umidificador

#define PIN_AR_CONDICIONADO 9  // Pino de controle do ar-condicionado (para temperatura)

#define PIN_AR_UMIDADE 10      // Pino de controle do ar-condicionado (para umidade)


// Parâmetros para controle de umidade

#define UMIDADE_MAXIMA 90     // Limite superior da umidade (aciona o ar-condicionado para umidade)   ideal = 60

#define UMIDADE_IDEAL 85      // Valor alvo de umidade ideal                                          ideal = 55

#define UMIDADE_MINIMA 83     // Limite inferior da umidade (aciona o umidificador)                   ideal = 50

#define TEMPO_LEITURA_UMIDADE 2000 // Intervalo mínimo entre leituras de umidade (em milissegundos)


// Parâmetros para controle de temperatura

#define TEMPERATURA_LIMITE 28 // Temperatura máxima antes de ligar o ar-condicionado (para temperatura)

#define TEMPO_LEITURA_TEMPERATURA 15000 // Intervalo mínimo entre leiturrs de temperatura (em milissegundos)


// Inicializa o sensor DHT

DHT dht(DHTPIN, DHTTYPE);


// Variáveis globais

float umidadeAtual;       // Última leitura de umidade

float temperaturaAtual;   // Última leitura de temperatura

int estadoUmidade = 0;    // Estado atual do sistema de umidade: 0 = ideal, -1 = umidificador, 1 = ar-condicionado (umidade)

bool arCondicionadoTemperaturaLigado = false; // Estado do ar-condicionado para controle de temperatura

unsigned long ultimaLeituraUmidade = 0; // tempo da última leitura de umidade

unsigned long ultimaLeituraTemperatura = 0; // Tempo da última leitura de temperatura


void setup() {

  // Configuração inicial

  Serial.begin(9600);           // Inicializa a comunicação serial

  dht.begin();                  // Prepara o sensor DHT para leitura

  pinMode(PIN_AR_CONDICIONADO, OUTPUT);  // configura o pino do ar-condicionado (temperatura) como saída

  pinMode(PIN_AR_UMIDADE, OUTPUT);       // configura o pino do ar-condicionado (umidade) como saída

  pinMode(PIN_UMIDIFICADOR, OUTPUT);     // configura o pino do umidificador como saída

  Serial.println("Monitorando temperatura e umidade com DHT11...");

}


void loop() {

  // Leitura periódica de umidade

  if (millis() - ultimaLeituraUmidade > TEMPO_LEITURA_UMIDADE) {

    float leituraUmidade = dht.readHumidity(); // Obtém a leitura de umidade


    // Valida a leitura de umidade

    if (!isnan(leituraUmidade)) {

      umidadeAtual = leituraUmidade;  // Atualiza a variável de umidade

      Serial.print("Umidade atual: ");

      Serial.print(umidadeAtual);

      Serial.println("%");

      ultimaLeituraUmidade = millis(); // Atualiza o tempo da última leitura de umidade

    } else {

      Serial.println("Erro ao ler a umidade do sensor DHT11. Tentando novamente...");

    }

  }


  // Leitura periódica de temperatura

  if (millis() - ultimaLeituraTemperatura > TEMPO_LEITURA_TEMPERATURA) {

    float leituraTemperatura = dht.readTemperature(); // Obtém a leitura de temperatura


    // Valida a leitura de temperatura

    if (!isnan(leituraTemperatura)) {

      temperaturaAtual = leituraTemperatura;  // Atualiza a variável de temperatura

      Serial.print("Temperatura atual: ");

      Serial.print(temperaturaAtual);

      Serial.println("°C");

      ultimaLeituraTemperatura = millis(); // Atualiza o tempo da última leitura de temperatura

    } else {

      Serial.println("Erro ao ler a temperatura do sensor DHT11. Tentando novamente...");

    }

  }


  // Controle do sistema de umidade

  switch (estadoUmidade) {

    case 0: // Umidade ideal

      if (umidadeAtual < UMIDADE_MINIMA) {  // Umidade abaixo do mínimo

        estadoUmidade = -1;

        digitalWrite(PIN_UMIDIFICADOR, HIGH); // Liga o umidificador

        Serial.println("Umidificador ativado.");

      } else if (umidadeAtual > UMIDADE_MAXIMA) { // Umidade acima do máximo

        estadoUmidade = 1;

        digitalWrite(PIN_AR_UMIDADE, HIGH); // Liga o ar condicionado (umidade)

        Serial.println("Ar-condicionado ativado (umidade).");

      }

      break;


    case -1: // Umidificador ligado

      if (umidadeAtual >= UMIDADE_IDEAL) { // Umidade chegou ao nivel ideal

        estadoUmidade = 0;

        digitalWrite(PIN_UMIDIFICADOR, LOW); // Desliga o umidificador

        Serial.println("Umidificador desativado.");

      }

      break;


    case 1: // Ar-condicionado ligado (umidade)

      if (umidadeAtual <= UMIDADE_IDEAL) { // Umidade voltou ao nível ideal

        estadoUmidade = 0;

        digitalWrite(PIN_AR_UMIDADE, LOW); // Desliga o ar-condicionado(umidade)

        Serial.println("Ar-condicionado desativado (umidade).");

      }

      break;

  }


  // Controle independente do ar-condicionado para temperatura

  if (temperaturaAtual > TEMPERATURA_LIMITE && !arCondicionadoTemperaturaLigado) {

    digitalWrite(PIN_AR_CONDICIONADO, HIGH); // Liga o ar-condicionado(temperatura)

    arCondicionadoTemperaturaLigado = true;

    Serial.println("Ar-condicionado ativado (temperatura).");

  } else if (temperaturaAtual <= TEMPERATURA_LIMITE && arCondicionadoTemperaturaLigado) {

    digitalWrite(PIN_AR_CONDICIONADO, LOW); // Desliga o ar-condicionadtemperatura)

    arCondicionadoTemperaturaLigado = false;

    Serial.println("Ar-condicionado desativado (temperatura).");

  }

}
