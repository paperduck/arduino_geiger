
const unsigned short sensor_pin = A0;
const unsigned short photo_pin = A5;
const unsigned short sound_pin = 2;
float                cur_sensor_val = 0;
float                photo_raw = 0;
float                temperature = 0;
// timestamps
unsigned long        sensor_time_ms = 0;
unsigned long        photo_sensor_time_ms = 0;
unsigned long        click_time_ms = 0;
unsigned long        led_0_time_ms = 0;
unsigned long        led_1_time_ms = 0;
unsigned long        led_2_time_ms = 0;
unsigned long        led_3_time_ms = 0;

// led statuses
boolean              led_0_on = false;
boolean              led_1_on = false;
boolean              led_2_on = false;
boolean              led_3_on = false;

float                click_pitch = 50;
int                  sensor_delay_ms = 10;
float                click_delay_ms = 10000;
float                click_delay_min = 1;
float                click_delay_max = 2000;
float                temp_min = 20;
float                temp_max = 29;
float                temp_default = 24; // 
float                temp_offset = 0; // = default temp - actual temp
unsigned short       num_baseline_samples = 8;
int                  iter = 0;

void get_temp()
{
  cur_sensor_val = analogRead(sensor_pin);    
  temperature = cur_sensor_val * 5000 / 1024;
  temperature -= 500;
  temperature /= 10;
}

void get_photo()
{
  photo_raw = analogRead(photo_pin); 
}

void setup()
{
  // pin modes
  pinMode(sound_pin, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(9600);

  // initate timestamps
  sensor_time_ms       = millis();
  photo_sensor_time_ms = millis();
  click_time_ms        = millis();
  led_0_time_ms       = millis();
  led_1_time_ms       = millis();
  led_2_time_ms       = millis();
  led_3_time_ms       = millis();

  // signal start of setup
  tone(sound_pin, 500, 100);
  delay(100); // for tone

  // get baseline temperature in case we start in a hot or cold room
  temp_offset = 0;
  for (iter = 1; iter <= num_baseline_samples; iter++)
  {
    get_temp();
    temp_offset += temperature;
    if (1 < iter && iter < num_baseline_samples)
    {
      tone(sound_pin, 1000, 20);
      delay(20); // for tone
    }
    if (iter < num_baseline_samples)
    {
      delay(500);
    }
  }
  temp_offset /= num_baseline_samples; // avg current temperature
  temp_offset = temp_default - temp_offset; // default temp - avg temp

  // signal start of loop
  tone(sound_pin, 1000, 300);
  delay(300); // for tone
}

void do_click()
{
  if ( (millis() - click_time_ms) > click_delay_ms)
  {
    click_time_ms = millis();
    tone(sound_pin, click_pitch, 10);
  }  
}

void do_lights()
{
  if ( (millis() - led_0_time_ms) > 125 )
  {
    blink_led(10, led_0_on);  
    led_0_time_ms =  millis();
  }
  if ( (millis() - led_1_time_ms) > 250)
  {
    blink_led(11, led_1_on);  
    led_1_time_ms =  millis();
  }
  if ( (millis() - led_2_time_ms) > 500 )
  {
    blink_led(12, led_2_on); 
    led_2_time_ms =  millis(); 
  }
  if ( (millis() - led_3_time_ms) > 1000 )
  {
    blink_led(13, led_3_on);  
    led_3_time_ms =  millis();
  }
}

void blink_led(unsigned short pin, boolean & b)
{
  if (b)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }   
  b = !b;
}

void loop()
{
  // temperature sensor
  if ( (millis() - sensor_time_ms) > sensor_delay_ms)
  {
    sensor_time_ms = millis();   
    cur_sensor_val = analogRead(sensor_pin);

    get_temp();    
    temperature += temp_offset;
    if (temperature < temp_min)
    {
      click_delay_ms = temp_min;
    }
    else
    {
      if (temperature > temp_max)
      {
        click_delay_ms = temp_max;
      }
      else
      {
        click_delay_ms = temperature;
      }
    }
    click_delay_ms -= temp_min;
    click_delay_ms /= (temp_max - temp_min);
    click_delay_ms = 1 - click_delay_ms;
    click_delay_ms *= (click_delay_max - click_delay_min);
    click_delay_ms += click_delay_min;

    //Serial.print("temperature: ");
    //Serial.print(temperature);
    //Serial.print("\n");
    //Serial.print("raw sensor value: ");
    //Serial.print(cur_sensor_val);
    //Serial.print("\n");
    //Serial.print("click_delay_ms: ");
    //Serial.print(click_delay_ms);
    //Serial.print("\n");
    //Serial.print("temperature: ");
    //Serial.print(temperature);
    //Serial.print("\n");
    //Serial.print("temp_offset: ");
    //Serial.print(temp_offset);
    //Serial.print("\n");
  }

  // photoresistor
  if ( (millis() - photo_sensor_time_ms) > sensor_delay_ms)
  {
    photo_sensor_time_ms = millis();
    get_photo(); // sets photo_raw = 0 to 1023
    click_pitch = (photo_raw / 1023);
    click_pitch = 1 - click_pitch;
    click_pitch = (click_pitch * 2000) + 50;

    Serial.print("photo_raw: ");
    Serial.print(photo_raw);
    Serial.print("\n");
  }

  do_click();

  do_lights();
}









