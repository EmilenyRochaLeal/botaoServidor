
# 💡 Projeto: Monitoramento de Botão com Raspberry Pi Pico W e Servidor Web

Este projeto consiste em uma solução IoT que **monitora o estado de um botão físico A ** conectado a uma **Raspberry Pi Pico W**, enviando os dados via Wi-Fi para um **servidor Node.js**. O estado do botão é exibido em tempo real em uma interface web.

---

## 🧱 Estrutura do Projeto

```
led_control_webserver/
├── server/
│   ├── public/
│   │   ├── index.html
│   │   ├── style.css
│   └── server.ts
├── src/
│   ├── led_control_webserver.c
│   ├── utils/
│   │   └── send-data-to-server/
│   │       ├── send-data.c
│   │       └── send-data.h
│   └── wifi-connection/
│       ├── wifi-connection.c
│       └── wifi-connection.h
```

---

## ⚙️ Componentes utilizados

- Raspberry Pi Pico W
- Botão físico A gpio 5
- Servidor local com Node.js
- Wi-Fi 2.4GHz

---

## 🚀 Como executar

### 🧩 1. Rodar o servidor Node.js

1. Acesse a pasta `server/`
2. Instale as dependências:
   ```bash
   npm install
   ```
3. Inicie o servidor:
   ```bash
   npx tsx server.ts 
   ```
4. Acesse a interface no navegador:
   ```
   http://localhost:3000
   ```

> A página exibirá o estado do botão recebido da placa em tempo real.

---

### 🔌 2. Configurar o firmware da placa

1. No código C (`led_control_webserver.c`), certifique-se de definir corretamente o IP do       servidor onde o Node.js está rodando:

2. em `send-data.c` verifique : 
    ```c
    #define SERVER_IP "192.168.1.xx"  // IP da sua máquina local
    #define SERVER_PORT 3000
    #define SERVER_PATH "/receber"
    ```

3. Use o comando `ip a` no Linux para descobrir o IP atual da máquina.

4. Coloque a placa em modo bootsel 

5. Compile e envie o arquivo de build terminado em .uf2 para a placa

6. Abra o serial monitor e aperte Start Monitoring 

7. Visualize no navegador

---

## 🌐 Comunicação

- A Pico W envia `POST /receber` com o estado do botão a cada 1 segundo.
- O servidor armazena o último estado e o disponibiliza via `GET /status`
- A interface web (`index.html`) faz fetch a cada 1 segundo para atualizar a tela.

---

## 🧪 Exemplo de payload enviado pela Pico W

```json
{
  "dado": 1 // 1 botão apertado e 0 solto
}
```

---

## 📸 Interface Web

- Feita com HTML, CSS e JS simples.
- Exibe o estado do botão (Pressionado/Solto) em tempo real.
- Usa `fetch()` e `setInterval()` para comunicação com o servidor.

---

## 👨‍💻 Autora

Projeto desenvolvido por Emileny 2025