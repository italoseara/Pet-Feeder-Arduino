#include <Servo.h>

const int delay_ = 100;

const int ledComida1 = 2;
const int ledComida2 = 3;
const int ledComida3 = 4;
const int ledAgua1 = 10;
const int ledAgua2 = 9;
const int ledAgua3 = 8;

const int ledOn = 5;

const int buttonComida = 13;
const int buttonAgua = 12;
const int buttonToggle = 11;

const int pinServoAgua = 6;
const int pinServoComida = 7;
const int forceSensorAgua = A2;
const int forceSensorComida = A1;

Servo servoAgua;
Servo servoComida;

int ledAgua = 0;
int ledComida = 0;
bool onOff = false;

class Time {
    public:
        int horas = 0;
        int minutos = 0;
        int segundos = 0;
};

int quantidadeAgua;
int quantidadeComida;

Time frequenciaComida;
Time frequenciaAgua;

Time frequenciaComidaSave;
Time frequenciaAguaSave;

int contador = 0;
int contadorAgua = 0;
int contadorComida = 0;

int sensorAgua;
int sensorComida;

float tigelaAgua;
float tigelaAguaNewtons;

float tigelaComida;
float tigelaComidaNewtons;

// EDITAR ANTES DE USAR:
const int margemErro = 10; // Margem de erro (gramas)
const float tigelaTara = 150; // Peso da tigela (gramas)
const float tigelaMax = 500; // Quantidade de ração máxima na tigela (gramas)

void setup() { 
    Serial.begin(9600);
    
    pinMode(ledAgua1, OUTPUT);
    pinMode(ledAgua2, OUTPUT);
    pinMode(ledAgua3, OUTPUT);

    pinMode(ledComida1, OUTPUT);
    pinMode(ledComida2, OUTPUT);
    pinMode(ledComida3, OUTPUT);

    pinMode(buttonAgua, INPUT);
    pinMode(buttonComida, INPUT);
    
    pinMode(ledOn, OUTPUT);
    pinMode(buttonToggle, INPUT);
    pinMode(forceSensorComida, INPUT);
    
    servoAgua.attach(pinServoAgua);
    servoAgua.write(0);

    servoComida.attach(pinServoComida);
    servoComida.write(0);
}

void loop() {
    
    delay(delay_/2);
    
    // Liga/Desliga
    if (digitalRead(buttonToggle)) {
    	onOff = !onOff;
    }
    
    if (onOff) {
        digitalWrite(ledOn, HIGH);
    } else {
        digitalWrite(ledOn, LOW);
    }
    
    digitalWrite(ledAgua, LOW);
    digitalWrite(ledComida, LOW);
    // Se estiver ligado
    if (onOff) {
        
        sensorAgua = analogRead(forceSensorAgua);
        sensorComida = analogRead(forceSensorComida);
        // Calculo aproximado para newtons
        tigelaAguaNewtons = exp((sensorAgua-555.53)/156.34);
        tigelaComidaNewtons = exp((sensorComida-555.53)/156.34);
        // Conversão para gramas
        tigelaAgua = tigelaAguaNewtons*101.97162;
        tigelaComida = tigelaComidaNewtons*101.97162;

        // PREDEFINIÇÕES COMIDA:
        if (digitalRead(buttonComida)) {
            contadorComida++;

            if (contadorComida == 1 || contadorComida == 4) { // Predefinição 1
                if (contadorComida == 4) {
                    contadorComida = 1;
                }

                ledComida = ledComida1;

                quantidadeComida = 150;
                frequenciaComida.horas = 0;
                frequenciaComida.minutos = 0;
                frequenciaComida.segundos = 10;

                frequenciaComidaSave = frequenciaComida;

            } else if (contadorComida == 2) { // Predefinição 2
                ledComida = ledComida2;
                quantidadeComida = 270;

                frequenciaComida.horas = 4;
                frequenciaComida.minutos = 0;
                frequenciaComida.segundos = 0;
                frequenciaComidaSave = frequenciaComida;

            } else if (contadorComida == 3) { // Predefinição 2
                ledComida = ledComida3;
                quantidadeComida = 400;

                frequenciaComida.horas = 4;
                frequenciaComida.minutos = 0;
                frequenciaComida.segundos = 0;
                frequenciaComidaSave = frequenciaComida;
            }
        }

        digitalWrite(ledComida, HIGH);

        // PREDEFINIÇÕES AGUA:
        if (digitalRead(buttonAgua)) {
            contadorAgua++;

            if (contadorAgua == 1 || contadorAgua == 4) { // Predefinição 1
                if (contadorAgua == 4) {
                    contadorAgua = 1;
                }

                ledAgua = ledAgua1;

                quantidadeAgua = 150;
                frequenciaAgua.horas = 0;
                frequenciaAgua.minutos = 0;
                frequenciaAgua.segundos = 10;

                frequenciaAguaSave = frequenciaAgua;

            } else if (contadorAgua == 2) { // Predefinição 2
                ledAgua = ledAgua2;
                quantidadeAgua = 270;

                frequenciaAgua.horas = 4;
                frequenciaAgua.minutos = 0;
                frequenciaAgua.segundos = 0;
                frequenciaAguaSave = frequenciaAgua;

            } else if (contadorAgua == 3) { // Predefinição 2
                ledAgua = ledAgua3;
                quantidadeAgua = 400;

                frequenciaAgua.horas = 4;
                frequenciaAgua.minutos = 0;
                frequenciaAgua.segundos = 0;
                frequenciaAguaSave = frequenciaAgua;
            }
        }

        digitalWrite(ledAgua, HIGH);
        
        // Contador regressivo (Comida)
        if (contador % (1000/delay_) == 0 && ledComida) {
            contador = 0;
            
            if (frequenciaComida.segundos > 0) {
                frequenciaComida.segundos--;
            } else {
                frequenciaComida.segundos = 59;
                if (frequenciaComida.minutos > 0) {
                    frequenciaComida.minutos--;
                } else {
                    frequenciaComida.minutos = 59;
                    if (frequenciaComida.horas > 0) {
                        frequenciaComida.horas--;
                    } else {
                    	// Reabastece
                        servoComida.write(90);
                        
                        // Espera a tigela encher
                        while (
                            tigelaComida - tigelaTara < quantidadeComida - margemErro && 
                            tigelaComida - tigelaTara < tigelaMax - margemErro
                        ) {
                            
                            sensorComida = analogRead(forceSensorComida);
                            // Calculo aproximado para newtons
                            tigelaComidaNewtons = exp((sensorComida-555.53)/156.34);
                            // Conversão para gramas
                            tigelaComida = tigelaComidaNewtons*101.97162;
                            
                            delay(delay_);
                        }
                        
                        // Volta para a posição inicial
                        servoComida.write(0);
                        
                        // Reinicia a contagem
                        frequenciaComida = frequenciaComidaSave;
                    }
                }
            }
        }

        // Contador regressivo (Água)
        if (contador % (1000/delay_) == 0 && ledAgua) {
            contador = 0;
            
            if (frequenciaAgua.segundos > 0) {
                frequenciaAgua.segundos--;
            } else {
                frequenciaAgua.segundos = 59;
                if (frequenciaAgua.minutos > 0) {
                    frequenciaAgua.minutos--;
                } else {
                    frequenciaAgua.minutos = 59;
                    if (frequenciaAgua.horas > 0) {
                        frequenciaAgua.horas--;
                    } else {
                    	// Reabastece
                        servoAgua.write(90);
                        
                        // Espera a tigela encher
                        while (
                            tigelaAgua - tigelaTara < quantidadeAgua - margemErro && 
                            tigelaAgua - tigelaTara < tigelaMax - margemErro
                        ) {
                            
                            sensorAgua = analogRead(forceSensorAgua);
                            // Calculo aproximado para newtons
                            tigelaAguaNewtons = exp((sensorAgua-555.53)/156.34);
                            // Conversão para gramas
                            tigelaAgua = tigelaAguaNewtons*101.97162;
                            
                            delay(delay_);
                        }
                        
                        // Volta para a posição inicial
                        servoAgua.write(0);
                        
                        // Reinicia a contagem
                        frequenciaAgua = frequenciaAguaSave;
                    }
                }
            }
        }
        contador++;
    }
    
    // Piscar led quando faltar 1 hora
    // para a reabastecimento
    if (contador % 10 == 0 && ledComida && frequenciaComida.horas == 0) {
        digitalWrite(ledComida, LOW); }
    if (contador % 10 == 0 && ledAgua && frequenciaAgua.horas == 0) {
        digitalWrite(ledAgua, LOW); }

    delay(delay_/2);

    if (contador % 10 == 0 && ledComida && frequenciaComida.horas == 0) {
        digitalWrite(ledComida, HIGH); }
    if (contador % 10 == 0 && ledAgua && frequenciaAgua.horas == 0) {
        digitalWrite(ledAgua, HIGH); }

    // Debug
    Serial.print("Comida: ");
    Serial.print(frequenciaComida.horas);
    Serial.print(":");
    Serial.print(frequenciaComida.minutos);
    Serial.print(":");
    Serial.print(frequenciaComida.segundos);
    Serial.print(" | ");
    Serial.print("Agua: ");
    Serial.print(frequenciaAgua.horas);
    Serial.print(":");
    Serial.print(frequenciaAgua.minutos);
    Serial.print(":");
    Serial.print(frequenciaAgua.segundos);
    Serial.print(" | ");
    if (onOff) {
        Serial.println("ON");
    } else {
        Serial.println("OFF");
    }
}