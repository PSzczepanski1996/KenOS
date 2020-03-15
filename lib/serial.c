#include <sys.h>
#define PORT 0x3f8   /* COM1 */
#include <vga.h>

void init_serial(){
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 1, 0x01);
   inb(0x3f8);
}

int is_transmit_empty(){
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a){
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}

int serial_received(){
   return inb(PORT + 5) & 1;
}
 
char read_serial(){
   while (serial_received() == 0);
 
   return inb(PORT);
}

void write_serialstring(const char *data){
    for (size_t i = 0; i < strlen(data); i++)
        write_serial(data[i]);
}

newSerialCommand = 1;
char serialCommandBuffer[256];

void serial_handler(struct regs *r){
    char key = read_serial();
    if(newSerialCommand){
       write_serialstring("discorify: {");
       newSerialCommand = 0;
    }
    else if(key = '\n'){
      serialCommandBuffer = '';
      write_serial('}\n');
      newSerialCommand = 1;
   } else {
      serialCommandBuffer += key;
      write_serial(key);
   }
}

/* Installs the serial handler into IRQ4.*/
void serial_install(){
    irq_install_handler(4, serial_handler);
    init_serial();
}