#ifndef _SERIALCOMHANDLER_H_
#define _SERIALCOMHANDLER_H_

/** Serial Com Wrapper Class
 * 
 * Since the com. with the controller of the urea device is conducted throu UART device,
 * it cannot communicate with PC.
 * Therfore, if you modify the program and hardware so as to com. with the controller thru Software Serial,
 * what you should do is just only modifying this class.
 */
class SerialComHandler
{
private:
  bool init_f = false;

public:
  SerialComHandler();
  ~SerialComHandler();
  void init(int baudrate = 9600);
  bool isInitiated();
  int available();
  unsigned char read();
  void write(unsigned char data);
};

#endif
