<h1 align="center">🚪 OcupaSala</h1>

<p align="center">
  <b>Monitor de ocupação das salas de estudo da biblioteca UVA</b><br>
  Sistema embarcado com Arduino que conta, em tempo real, quantas pessoas entram e saem de uma sala e avisa quando a lotação é atingida.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Arduino-Uno%20R3-00979D?logo=arduino&logoColor=white" alt="Arduino Uno R3">
  <img src="https://img.shields.io/badge/linguagem-C%2FC%2B%2B-00599C?logo=cplusplus&logoColor=white" alt="C/C++">
  <img src="https://img.shields.io/badge/Bluetooth-HC--05-0082FC?logo=bluetooth&logoColor=white" alt="Bluetooth HC-05">
  <img src="https://img.shields.io/badge/simulação-Tinkercad-F15A24?logo=autodesk&logoColor=white" alt="Tinkercad">
  <img src="https://img.shields.io/badge/status-em%20desenvolvimento-yellow" alt="Status">
</p>

<p align="center">
  <img src="media/demo-maquete.gif" alt="Demonstração da maquete do OcupaSala" height="380">
  &nbsp;&nbsp;
  <img src="media/demo-circuito.gif" alt="Teste do circuito na protoboard" height="380">
</p>

---

## 📌 Sumário

- [O problema](#-o-problema)
- [A solução](#-a-solução)
- [Como funciona](#️-como-funciona)
- [Hardware](#-hardware)
- [Esquema de ligações](#-esquema-de-ligações)
- [Código](#-código)
- [Como executar](#️-como-executar)
- [Testes realizados](#-testes-realizados)
- [Estrutura do repositório](#-estrutura-do-repositório)
- [Cronograma](#-cronograma)
- [Equipe](#-equipe)

---

## ❓ O problema

Na biblioteca da **Universidade Veiga de Almeida (UVA)**, as salas de estudo em grupo funcionam por controle manual: o aluno pede a chave ao porteiro, que a libera. Depois disso, **o porteiro não tem como saber quantas pessoas realmente estão usando a sala**. É comum o aluno que retirou a chave ser acompanhado por colegas que chegam depois, e a sala acaba superlotada sem que ninguém perceba.

Isso gera desconforto para os usuários e dificulta a fiscalização, que hoje depende só da observação presencial de um porteiro que também precisa atender outras demandas.

## 💡 A solução

O **OcupaSala** é um contador de pessoas instalado na porta da sala. Dois sensores infravermelhos identificam o **sentido da passagem** (entrada ou saída), o Arduino mantém a contagem atualizada e:

- 🟢 **LED verde** aceso enquanto há vagas;
- 🔴 **LED vermelho** + **alarme sonoro** quando a capacidade máxima é atingida;
- 🔊 **bipes diferentes** para entrada (agudo) e saída (grave);
- 📡 **envio dos eventos** pela serial / Bluetooth para um painel de monitoramento do porteiro.

**Público-alvo:** porteiro e equipe da biblioteca, responsáveis pelo controle das chaves e pela fiscalização do uso das salas.

## ⚙️ Como funciona

```
 [Sensor de entrada] ─┐                    ┌─► LED verde    (há vagas)
                      ├─► [Arduino Uno] ───┼─► LED vermelho (sala cheia)
 [Sensor de saída]  ──┘         │          └─► Buzzer       (bipes e alarme)
                                │
                                └─► Serial / Bluetooth HC-05 ─► Dashboard do porteiro
```

1. **Inicialização** — configura os pinos, abre a serial a 9600 bit/s e começa com a sala vazia (0 pessoas).
2. **Detecção** — lê os dois sensores e só considera uma detecção no instante em que o sensor muda de *livre* para *detectado* (borda), evitando contar várias vezes uma pessoa parada na frente do sensor.
3. **Sentido da passagem** — o primeiro sensor acionado define o sentido:
   - Sensor 1 → Sensor 2 = **entrada** (`+1`)
   - Sensor 2 → Sensor 1 = **saída** (`−1`, nunca abaixo de 0)
4. **Anti contagem dupla** — depois de contar uma passagem, o segundo sensor dela é ignorado até ser acionado ou até passarem **2 segundos**.
5. **Sinalização** — atualiza LEDs e buzzer de acordo com a lotação.
6. **Monitoramento** — cada evento é enviado pela serial, por exemplo:
   ```
   Ocupação: ENTRADA - 3 PESSOAS
   Ocupação: SAÍDA - 2 PESSOAS
   ```

### Regras de lotação

| Condição | Indicação visual | Som / registro |
|---|---|---|
| Entrada detectada | Contagem +1 | Bip agudo (2500 Hz) + registro serial |
| Saída detectada | Contagem −1 (mínimo 0) | Bip grave (400 Hz) + registro serial |
| Ocupação abaixo de 5 | 🟢 Verde ligado · vermelho desligado | Sem alarme |
| Ocupação ≥ 5 | 🔴 Vermelho ligado · verde desligado | Alarme de 1500 Hz a cada ~500 ms |

> O limite de **5 pessoas** é o valor usado nos testes da maquete e pode ser alterado na constante `LOTACAO_MAXIMA`. O sistema alerta, mas não bloqueia fisicamente a entrada; por isso a contagem pode passar de 5.

## 🔩 Hardware

| Qtd. | Componente | Função |
|---|---|---|
| 1 | Arduino Uno R3 + cabo USB | Microcontrolador |
| 2 | Sensor infravermelho TCRT5000 | Detectar a passagem na porta (sensores) |
| 1 | LED difuso 5 mm verde | Indicar vagas disponíveis (atuador) |
| 1 | LED difuso 5 mm vermelho | Indicar sala cheia (atuador) |
| 1 | Buzzer 5 V | Bipes e alarme de lotação (atuador) |
| 1 | Módulo Bluetooth HC-05 | Comunicação sem fio com o dashboard |
| 2 | Resistor 220 Ω | Limitar corrente dos LEDs |
| 1 + 1 | Resistores 1 kΩ e 2 kΩ | Divisor de tensão 5 V → 3,3 V no RX do HC-05 |
| 1 | Protoboard 830 pontos + jumpers | Montagem |
| 1 | Fonte 9 V (bateria ou P4) | Alimentação autônoma |
| — | Placas para a maquete | Estrutura física representando a sala |

## 🔌 Esquema de ligações

| Componente | Pino do Arduino |
|---|---|
| Sensor de entrada (TCRT5000) | D2 |
| Sensor de saída (TCRT5000) | D3 |
| LED verde (+ 220 Ω) | D9 |
| LED vermelho (+ 220 Ω) | D10 |
| Buzzer | D11 |
| HC-05 TX → RX do Arduino | <!-- TODO: confirmar pino --> |
| HC-05 RX ← TX do Arduino (via divisor 1 kΩ / 2 kΩ) | <!-- TODO: confirmar pino --> |
| Alimentação 9 V | VIN e GND |

> O circuito foi simulado primeiro no **Autodesk Tinkercad** (gêmeo digital) e depois montado fisicamente.
<!-- TODO: adicionar o link do projeto no Tinkercad e um print do esquema em media/ -->

## 💻 Código

O firmware está em [`firmware/ocupasala/ocupasala.ino`](firmware/ocupasala/ocupasala.ino). Os principais parâmetros ficam no topo do arquivo:

```cpp
const int LOTACAO_MAXIMA = 5;              // capacidade da sala
const int DETECTADO      = LOW;            // troque para HIGH se o seu sensor for invertido
const unsigned long TEMPO_PASSAGEM = 2000; // ms para atravessar os dois sensores
```

## ▶️ Como executar

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software).
2. Clone o repositório:
   ```bash
   git clone https://github.com/<seu-usuario>/ocupasala.git
   ```
3. Abra `firmware/ocupasala/ocupasala.ino` na Arduino IDE.
4. Selecione **Placa: Arduino Uno** e a porta correta em *Ferramentas*.
5. Clique em **Carregar**.
6. Abra o **Monitor Serial** em **9600 baud** e passe a mão (ou um bonequinho 🙂) pelos sensores.

### Dashboard

<!-- TODO: descrever o dashboard entregue em 08/10 (tecnologia, como conectar pelo Bluetooth, print da tela) e colocar o código na pasta dashboard/ -->
A interface de monitoramento fica na pasta [`dashboard/`](dashboard/).

## 🧪 Testes realizados

| Ensaio | Resultado |
|---|---|
| Partida com a sala vazia | Contagem em 0; LED verde ligado ✅ |
| Passagem pelo sensor de entrada | Contagem +1; bip agudo ✅ |
| Passagem pelo sensor de saída | Contagem −1; bip grave ✅ |
| Saída com contagem em zero | Contagem permanece em 0 ✅ |
| Chegada da 5ª pessoa | Vermelho acende, verde apaga, alarme inicia ✅ |
| Saída depois de lotar | Sinalização de sala cheia desliga abaixo de 5 ✅ |
| Dois sensores na mesma passagem | Segundo sensor ignorado, sem contagem dupla ✅ |

### Problemas encontrados e soluções

| Problema | Solução |
|---|---|
| Componentes físicos atrasaram na entrega | Desenvolvimento e testes antecipados em um gêmeo digital no Tinkercad |
| Pessoa contada duas vezes ao atravessar os dois sensores | Bloqueio temporário do segundo sensor da passagem (até 2 s) |
| Sensor mantido ativo gerando várias contagens | Detecção por borda (estado anterior de cada sensor) + intervalo de 50 ms no loop |
| HC-05 trabalha com 3,3 V no RX e o Arduino envia 5 V | Divisor de tensão resistivo 1 kΩ / 2 kΩ (≈ 3,33 V) |
| Sensores precisam ficar alinhados com a porta | Ajuste da posição dos sensores na abertura da maquete |

## 📁 Estrutura do repositório

```
ocupasala/
├── firmware/
│   └── ocupasala/
│       └── ocupasala.ino        # código do Arduino (C/C++)
├── dashboard/                    # interface de monitoramento
├── docs/
│   ├── diario-de-bordo.pdf       # documentação completa da montagem e dos testes
│   └── entregas/                 # PDFs entregues em cada etapa da disciplina
├── media/                        # fotos e GIFs de demonstração
└── README.md
```

## 🗓️ Cronograma

| Data | Entrega | Status |
|---|---|---|
| 27/08 | Formação da equipe, tema e problema | ✅ |
| 02/09 | Canvas, lista de componentes e diagrama em blocos | ✅ |
| 10/09 | Checkpoint 1 – projeto eletrônico e esquema de ligações | ✅ |
| 15/09 | Checkpoint 2 – código inicial, testes e dificuldades | ✅ |
| 24/09 | Integração sensor + atuador, protótipo eletrônico funcionando | ✅ |
| 01/10 | Estrutura física do protótipo (maquete) | ✅ |
| 08/10 | Dashboard / interface Serial | ✅ |
| 22/10 | Protótipo completo + vídeo de demonstração (2 min) | ⏳ |
| 29/10 | Exposição oficial na Semana da Computação | ⏳ |
| 30/11 | Relatório técnico completo e versão final | ⏳ |

## 👥 Equipe

| Integrante | Responsabilidade |
|---|---|
| **Adriel Barbosa de Freitas** | Documentação |
| **Camile Pimenta Monteiro** | Bluetooth e comunicação · diagramas e documentação · dashboard |
| **Daniel Salgueiro Puga Paiva de Andrade** | Testes e integração |
| **Fabio Cezar Paiva Paixão** | Testes e integração · diagramas e documentação |
| **Gustavo Pereira Fernandes Souto** | Testes e integração · Diagramas e documentação |
| **Letícia dos Reis Prado** | Programação e montagem do Arduino · Documentação · Bluetooth e comunicação |

Todos participaram da montagem do hardware, da integração e da apresentação.

---

<p align="center">
  Projeto acadêmico da disciplina de <b>Sistemas Embarcados</b><br>
  Universidade Veiga de Almeida (UVA) · Rio de Janeiro · 2026
</p>
