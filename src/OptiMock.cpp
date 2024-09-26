
void TT_RigidBodyLocation(int rbIndex,                  //== RigidBody Index
                          float *x, float *y, float *z, //== Position
                          float *qx, float *qy, float *qz,
                          float *qw,                             //== Quaternion
                          float *yaw, float *pitch, float *roll) //== Euler
{
  (void)rbIndex;
  *x = 0;
  *y = 0;
  *z = 0;
  *qx = *qy = *qz = *qw = 0;
  *yaw = *pitch = *roll = 0;
}

