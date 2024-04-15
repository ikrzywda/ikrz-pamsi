# Pakiety

## Specyfikacja

### Transmisja

#### Specyfikacja

- Wejście: Wiadomość $m$ o długości $n$ bajtów
- Wyjście: Lista pakietów $P$, gdzie $|P| = \frac{n}{p} + 2$, gdzie $p$ to długość payloadu pakietu w bajtach.

#### Opis

Przy zadanym $p \in N$ określającym maksymalną długość payloadu w bajtach, dzielimy wiadomość $m$ na części $P_i$, gdzie $|P_i| < p$.

#### Pseudokod

```c
typedef enum PacketType {
    START, END, PAYLOAD
}

typedef struct {
    PacketType type;
    unsigned long range_start;
    unsigned long range_end;
    usize_t message_length;
    usize_t payload_length;
    uint8_t *payload;
} Packet;

int transmit_messages(Packet *packet_buffer, uint8_t *message, unsigned int packet_length) {
    usize_t message_length = sizeof(message);
    unsigned int message_count = sizeof(message_length) / packet_buffer + (message_length % packet_length > 0 ? 1 : 0);

    packet_buffer = malloc(sizeof(Packet) * message_count);

    for (int i = 1; i < message_count + 1; i++) {
        if (i == message_count - 1) {
            packet_buffer[i - 1] = packet_init(message[(i - 1) * p : i * p + message_length % packet_count]);
            continue;
        }
        packet_buffer[i - 1] = packet_init(message[(i - 1) * p : i * p + message_length % packet_count]);
    }

    return 0;
}

```

#### Złozoność obliczeniowa:

$\mathcal{O}(n) = \frac{n}{p} = n$, gdzie $n$ to długość wiadomości, a $p$ to długość payloadu pakietu

#### Złozoność pamięciowa:

$\mathcal{O}(n) = n$, gdzie $n$ to długość wiadomości

### Odbiór

#### Specyfikacja

- Wejście: Zbiór pakietów $P$, gdzie $P_1$ to wiadomość startowa a $P_n$ to wiadomość kończąca, gdzie $n = |P|$
- Wyjście: Wiadomość $m$ o długości $n$ bajtów

#### Opis

Na początku programu inicjalizujemy stos $S$ i wkładamy na stos kazdą przychodzącą wiadomość (w uproszczonym modelu ładujemy zadany zbiór pakietów do stosu). Po otrzymaniu wiadomości kończącej, alokujemy blok pamięci $B$ odpowiadający długości wiadomości w pakietach. Iterując po stosie jednocześnie zdejmując z niego wiadomości, kopiujemy zawartość payloadów na odpowiednie zakresy w bloku $B$.

#### Pseudokod

```c
typedef struct {
  unsigned int size;
  unsigned int capacity;
  Packet *buffer;
} Stack;

int _receiver_assemble_message(Stack *packet_stack, uint8_t *message) {
    for (int i = 0; i < packet_stack.size, i++) {
        memcpy(message + packet.range_start, &packet.payload, packet.range_end - packet.range_start);
    }
    return 0;
}

int receiver_assemble_message(uint8_t *message) {
    Stack packet_stack;
    bool is_initialized = false;
    // załozenie, ze receive message odbiera wiadomość
    for (packet in receiver_receive_packet()) {
        if (packet.type == START && !is_initialized) {
            init_stack(&packet_stack, capacity=packet.message_length);
            message = malloc(packet.message_length);
            continue;
        }
        if (!is_initialized) {
            continue;
        }
        if (packet.type == PAYLOAD) {
            packet_stack.push(packet);
        }
        if (packet.type == END) {
            return _receiver_assemble_message(packet_stack, message);
        }
    }
}
```

#### Złozoność obliczeniowa

$\mathcal{O}(n) = 2n = n$, gdzie $n$ to liczba pakietów

#### Złozoność pamięciowa

$\mathcal{O}(n) = 2n = n$, gdzie $n$ to rozmiar wiadomości
