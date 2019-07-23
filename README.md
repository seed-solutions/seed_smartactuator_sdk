# seed_smartactuator_sdk

## サンプルコード
ROS上のパッケージから本ライブラリをインクルードし、SEED-Driverへコマンドを送信するための一連サンプルを示す。    
なお、本サンプルの概要は下記の通りである。
1. シリアルポートを開く
2. 原点復帰（キャリブレーション）が記述されたスクリプト１番を実行する
3. スクリプトが終了するまで待機する
4. 現在値取得を送信し続ける

### C++
```c++
#include <ros/ros.h>
#include "seed_smartactuator_sdk/seed3_command.h"

int main(int argc, char **argv)
{
  ros::init(argc,argv,"cpp_sample_node");
  ros::NodeHandle nh;

  seed::controller::SeedCommand seed;

  seed.openPort("/dev/ttyACM0",115200);
  seed.openCom();

  int id = 1;
  std::array<int,3> motor_state;

  seed.runScript(id,1);
  seed.waitForScriptEnd(1);

  while (ros::ok())
  {
    motor_state = seed.getPosition(id);
    if(motor_state[0]) std::cout << motor_state[2] << std::endl;
  }
  return 0;
}
```

### Python

```python
#!/usr/bin/env python
import rospy
from seed_solutions_sdk.seed3_command import SeedCommand

#----------- main -----------------#
if __name__ == "__main__":
  rospy.init_node('python_sample_node')

  seed=SeedCommand()

  seed.open_port("/dev/ttyACM0",115200)
  seed.open_com()

  id = 1
  motor_state = []

  seed.run_script(id,1)
  seed.wait_for_script_end(1)
  
  while not rospy.is_shutdown():
    motor_state = seed.get_position(id)
    if(motor_state[0]): print motor_state[2]
```
