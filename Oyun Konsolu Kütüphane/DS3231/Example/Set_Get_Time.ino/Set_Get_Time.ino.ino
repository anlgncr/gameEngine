#include <DS3231.h>

DS3231 my_watch(0x68);
DS3231::dateTime my_dt;

void setup() {
  Serial.begin(9600);
  my_watch.setTime(19,23,0);
  my_watch.setDate(2,28,8,18);
}

void loop() {
  my_watch.getTime(&my_dt);
  my_watch.getDate(&my_dt);

  char buf[20];
  sprintf(buf,"%d : %d : %d", my_dt.hours, my_dt.minutes, my_dt.seconds);
  Serial.println(buf);

  sprintf(buf,"%d : %d : %d", my_dt.date, my_dt.month, my_dt.year+2000);
  Serial.println(buf);

  Serial.print("Temperature : ");
  Serial.println(my_watch.getTemperature());
  delay(1000);
}