void getEOP()
{
  int minL = 0;
  byte avrch = 0;
  byte bl = 0;
  while (!EOP_SEEN)
  {
    while (Serial.available() > 0)
    {
      byte ch = Serial.read();
      _buffer[iBuffer] = ch;
      iBuffer = (++iBuffer) % 256; // increment and wrap
      if (iBuffer == 1)
        avrch = ch; // save command
      if ((avrch == STK_PROG_PAGE) && (iBuffer == 3))
      {
        minL = 256 * _buffer[1] + _buffer[2] + 4;
      }
      if ((iBuffer > minL) && (ch == CRC_EOP))
      {
        EOP_SEEN = true;
      }
    }
    if (!EOP_SEEN)
    {
      heartbeat(); // light the heartbeat LED
      //     if (bl == 100) {
      //       pulse(LED_ERR,1,10);  // blink the red LED
      //       bl = 0;
      //     }
      bl++;
      delay(10);
    }
  }
}

// serialEvent not used so sketch would be compatible with older IDE versions
//void serialEvent() {
//  int minL = 0;
//  byte avrch = 0;
//  while (Serial.available()>0)
//  {
//    byte ch = Serial.read();
//    _buffer[iBuffer] = ch;
//    iBuffer = (++iBuffer)%256;  // increment and wrap
//    if (iBuffer == 1)  avrch = ch;  // save command
//    if ((avrch == STK_PROG_PAGE) && (iBuffer==3)) {
//      minL = 256*_buffer[1] + _buffer[2] + 4;
//    }
//    if ((iBuffer>minL) && (ch == CRC_EOP)) {
//      EOP_SEEN = true;
//    }
//  }
//}

//#define PTIME 20
void pulse(int pin, int times, int ptime)
{
  do
  {
    digitalWrite(pin, HIGH);
    delay(ptime);
    digitalWrite(pin, LOW);
    delay(ptime);
    times--;
  } while (times > 0);
}
void pulse(int pin, int times)
{
  pulse(pin, times, 50);
}

byte getch()
{
  if (pBuffer == iBuffer)
  { // spin until data available ???
    pulse(LED_ERR, 1);
    //    beep(1700, 20);
    error++;
    return -1;
  }
  byte ch = _buffer[pBuffer];  // get next char
  pBuffer = (++pBuffer) % 256; // increment and wrap
  return ch;
}

void readbytes(int n)
{
  for (int x = 0; x < n; x++)
  {
    buff[x] = getch();
  }
}

void spi_init()
{
  byte x;
  SPCR = 0x53;
#ifdef LOW_SPEED
  SPCR = SPCR | B00000011;
#endif
  x = SPSR;
  x = SPDR;
}

void spi_wait()
{
  do
  {
  } while (!(SPSR & (1 << SPIF)));
}

byte spi_send(byte b)
{
  byte reply;
#ifdef LOW_SPEED
  cli();
  CLKPR = B10000000;
  CLKPR = B00000011;
  sei();
#endif
  SPDR = b;
  spi_wait();
  reply = SPDR;
#ifdef LOW_SPEED
  cli();
  CLKPR = B10000000;
  CLKPR = B00000000;
  sei();
#endif
  return reply;
}

byte spi_transaction(byte a, byte b, byte c, byte d)
{
  byte n;
  spi_send(a);
  n = spi_send(b);
  //if (n != a) error = -1;
  n = spi_send(c);
  return spi_send(d);
}

void replyOK()
{
  //  if (EOP_SEEN == true) {
  if (CRC_EOP == getch())
  { // EOP should be next char
    Serial.write(STK_INSYNC);
    Serial.write(STK_OK);
  }
  else
  {
    pulse(LED_ERR, 2);
    Serial.write(STK_NOSYNC);
    error++;
  }
}

void breply(byte b)
{
  if (CRC_EOP == getch())
  { // EOP should be next char
    Serial.write(STK_INSYNC);
    Serial.write(b);
    Serial.write(STK_OK);
  }
  else
  {
    Serial.write(STK_NOSYNC);
    error++;
  }
}

void get_parameter(byte c)
{
  switch (c)
  {
  case 0x80:
    breply(HWVER);
    break;
  case 0x81:
    breply(SWMAJ);
    break;
  case 0x82:
    breply(SWMIN);
    break;
  case 0x93:
    breply('S'); // serial programmer
    break;
  default:
    breply(0);
  }
}

void set_parameters()
{
  // call this after reading paramter packet into buff[]
  param.devicecode = buff[0];
  param.revision = buff[1];
  param.progtype = buff[2];
  param.parmode = buff[3];
  param.polling = buff[4];
  param.selftimed = buff[5];
  param.lockbytes = buff[6];
  param.fusebytes = buff[7];
  param.flashpoll = buff[8];
  // ignore buff[9] (= buff[8])
  //getch(); // discard second value

  // WARNING: not sure about the byte order of the following
  // following are 16 bits (big endian)
  param.eeprompoll = beget16(&buff[10]);
  param.pagesize = beget16(&buff[12]);
  param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  param.flashsize = buff[16] * 0x01000000 + buff[17] * 0x00010000 + buff[18] * 0x00000100 + buff[19];
}

void start_pmode()
{
  spi_init();
  // following delays may not work on all targets...
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);
  pinMode(SCK, OUTPUT);
  digitalWrite(SCK, LOW);
  delay(50 + EXTRA_SPI_DELAY);
  digitalWrite(RESET, LOW);
  delay(50 + EXTRA_SPI_DELAY);
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
}

void end_pmode()
{
  pinMode(MISO, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(RESET, INPUT);
  pmode = 0;
}

void universal()
{
  //  int w;
  byte ch;
  //  for (w = 0; w < 4; w++) {
  //    buff[w] = getch();
  //  }
  readbytes(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

void flash(byte hilo, int addr, byte data)
{
  spi_transaction(0x40 + 8 * hilo, addr >> 8 & 0xFF, addr & 0xFF, data);
}
void commit(int addr)
{
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

//#define _current_page(x) (here & 0xFFFFE0)
int current_page(int addr)
{
  if (param.pagesize == 32)
    return addr & 0xFFFFFFF0;
  if (param.pagesize == 64)
    return addr & 0xFFFFFFE0;
  if (param.pagesize == 128)
    return addr & 0xFFFFFFC0;
  if (param.pagesize == 256)
    return addr & 0xFFFFFF80;
  return addr;
}
byte write_flash(int length)
{
  if (param.pagesize < 1)
  {
    return STK_FAILED;
  }
  //if (param.pagesize != 64) return STK_FAILED;
  int page = current_page(_addr);
  int x = 0;
  while (x < length)
  {
    if (page != current_page(_addr))
    {
      commit(page);
      page = current_page(_addr);
    }
    flash(LOW, _addr, buff[x++]);
    flash(HIGH, _addr, buff[x++]);
    _addr++;
  }
  commit(page);
  return STK_OK;
}

byte write_eeprom(int length)
{
  // here is a word address, so we use here*2
  // this writes byte-by-byte,
  // page writing may be faster (4 bytes at a time)
  for (int x = 0; x < length; x++)
  {
    spi_transaction(0xC0, 0x00, _addr * 2 + x, buff[x]);
    delay(45);
  }
  return STK_OK;
}

void program_page()
{
  byte result = STK_FAILED;
  int length = 256 * getch() + getch();
  if (length > 256)
  {
    Serial.write(STK_FAILED);
    error++;
    return;
  }
  char memtype = (char)getch();
  //  for (int x = 0; x < length; x++) {
  //    buff[x] = getch();
  //  }
  readbytes(length);
  if (CRC_EOP == getch())
  {
    Serial.write(STK_INSYNC);
    switch (memtype)
    {
    case 'E':
      result = (byte)write_eeprom(length);
      break;
    case 'F':
      result = (byte)write_flash(length);
      break;
    }
    Serial.write(result);
    if (result != STK_OK)
    {
      error++;
    }
  }
  else
  {
    Serial.write(STK_NOSYNC);
    error++;
  }
}

byte flash_read(byte hilo, int addr)
{
  return spi_transaction(0x20 + hilo * 8,
                         (addr >> 8) & 0xFF,
                         addr & 0xFF,
                         0);
}

char flash_read_page(int length)
{
  for (int x = 0; x < length; x += 2)
  {
    byte low = flash_read(LOW, _addr);
    Serial.write(low);
    byte high = flash_read(HIGH, _addr);
    Serial.write(high);
    _addr++;
  }
  return STK_OK;
}

char eeprom_read_page(int length)
{
  // here again we have a word address
  for (int x = 0; x < length; x++)
  {
    byte ee = spi_transaction(0xA0, 0x00, _addr * 2 + x, 0xFF);
    Serial.write(ee);
  }
  return STK_OK;
}

void read_page()
{
  byte result = (byte)STK_FAILED;
  int length = 256 * getch() + getch();
  char memtype = getch();
  if (CRC_EOP != getch())
  {
    Serial.write(STK_NOSYNC);
    return;
  }
  Serial.write(STK_INSYNC);
  if (memtype == 'F')
    result = flash_read_page(length);
  if (memtype == 'E')
    result = eeprom_read_page(length);
  Serial.write(result);
  return;
}

void read_signature()
{
  if (CRC_EOP != getch())
  {
    Serial.write(STK_NOSYNC);
    error++;
    return;
  }
  Serial.write(STK_INSYNC);
  byte high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  Serial.write(high);
  byte middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  Serial.write(middle);
  byte low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  Serial.write(low);
  Serial.write(STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////

////////////////////////////////////
////////////////////////////////////

int avrisp()
{
  byte data, low, high;
  byte avrch = getch();
  switch (avrch)
  {
  case STK_GET_SYNC: // get in sync
    replyOK();
    break;
  case STK_GET_SIGNON: // get sign on
    if (getch() == CRC_EOP)
    {
      Serial.write(STK_INSYNC);
      Serial.write("AVR ISP");
      Serial.write(STK_OK);
    }
    break;
  case STK_GET_PARM: // 0x41
    get_parameter(getch());
    break;
  case STK_SET_PARM: // 0x42
    readbytes(20);
    set_parameters();
    replyOK();
    break;
  case STK_SET_PARM_EXT: // extended parameters - ignore for now
    readbytes(5);
    replyOK();
    break;
  case STK_PMODE_START: // 0x50
    //     beep(2272, 20);
    start_pmode();
    replyOK();
    break;
  case STK_PMODE_END: //0x51
    //      beep(1912, 50);
    error = 0;
    end_pmode();
    replyOK();
    break;
  case STK_SET_ADDR: // 0x55
    _addr = getch() + 256 * getch();
    replyOK();
    break;
  case STK_UNIVERSAL: //UNIVERSAL 0x56
    universal();
    break;
  case STK_PROG_FLASH: //STK_PROG_FLASH  ???
    low = getch();
    high = getch();
    replyOK();
    break;
  case STK_PROG_DATA: //STK_PROG_DATA  ???
    data = getch();
    replyOK();
    break;
  case STK_PROG_PAGE: //STK_PROG_PAGE
    //      beep(1912, 20);
    program_page();
    break;
  case STK_READ_PAGE: //STK_READ_PAGE
    read_page();
    break;
  case STK_READ_SIGN: //STK_READ_SIGN
    read_signature();
    break;
  // expecting a command, not CRC_EOP
  // this is how we can get back in sync
  case CRC_EOP:
    Serial.write(STK_NOSYNC);
    break;
  // anything else we will return STK_UNKNOWN
  default:
    if (CRC_EOP == getch())
      Serial.write(STK_UNKNOWN);
    else
      Serial.write(STK_NOSYNC);
  }
}

// beep without using PWM
//void beep(int tone, long duration){
//  long elapsed = 0;
//  while (elapsed < (duration * 10000)) {
//    digitalWrite(PIEZO, HIGH);
//    delayMicroseconds(tone / 2);
//    digitalWrite(PIEZO, LOW);
//    delayMicroseconds(tone / 2);
// Keep track of how long we pulsed
//    elapsed += tone;
//  }
//}
