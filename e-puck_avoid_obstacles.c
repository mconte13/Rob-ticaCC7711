#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <webots/device.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>
#include <webots/motor.h>
#include <webots/nodes.h>
#include <webots/robot.h>

#define MAX_SPEED 6.28
#define TIME_STEP 64

// funcao principal
int main(int argc, char **argv) {
  // inicia Webots API
  wb_robot_init();

  // intervalo de variaveis
  int i;
  WbDeviceTag ps[8];
  char ps_names[8][4] = {
    "ps0", "ps1", "ps2", "ps3",
    "ps4", "ps5", "ps6", "ps7"
  };

  // inicia os dispositivos
  for (i = 0; i < 8 ; i++) {
    ps[i] = wb_robot_get_device(ps_names[i]);
    wb_distance_sensor_enable(ps[i], TIME_STEP);
  }

  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  // simulação de loop até que um evento de saída seja recebido
  while (wb_robot_step(TIME_STEP) != -1) {
    // le as saidas dos sensores
    double ps_values[8];
    for (i = 0; i < 8 ; i++)
      ps_values[i] = wb_distance_sensor_get_value(ps[i]);

    // deteccao de obstaculos - trabalhando apenas com on sensores frontais e laterais
    bool right_obstacle =
      ps_values[0] > 110.0 ||
      ps_values[1] > 110.0 ||
      ps_values[2] > 110.0 ;
    bool left_obstacle =
      ps_values[5] > 110.0 ||
      ps_values[6] > 110.0 ||
      ps_values[7] > 110.0 ;

    // inicializa o motor com velocidade 50% da MAX_SPEED.
    double left_speed  = 0.6 * MAX_SPEED;
    double right_speed = 0.6 * MAX_SPEED;

    // modifica a velocidade de acordo com o obstaculo
    if (left_obstacle) {
      // vira para direita
      left_speed  = 0.6 * MAX_SPEED;
      right_speed = -0.6 * MAX_SPEED;
    }
    else if (right_obstacle) {
      // vira para esquerda
      left_speed  = -0.6 * MAX_SPEED;
      right_speed = 0.6 * MAX_SPEED;
    }

    // write actuators inputs
    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }

  // limpa a Webots API
  wb_robot_cleanup();
  //exit
  return 0;
}