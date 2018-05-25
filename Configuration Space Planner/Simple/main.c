/****
 * 16-311 Lab-9
 * Basic Coordination Script for SCARA (Revolute-Revolute) Robot Arm, Oriented
 * Vertically (against gravity, requires compensation).
 * List of Target Points in X-Y Plane are Given. Visited by Navigating from Current
 * Position to Voronoi Roadmap (generated in accompanying MatLab files), to
 * Roadmap Vertex Closest to the Target Position, then to the Target Position.
 * Coordinate Spaces and Arm Orientation and Geometry are Further Outlined in
 * Configuration Space MatLab file, ConfigurationSpacePlanner.m .
****/

#define JointA motorA
#define JointB motorB

#define NUM_TARGETS 3
// List of Targets in X-Y Space, units inches.
float targs[][] = {
  {3,3},
  {4,4},
  {-3,3}
};

/**** -- PATH-PLANNING -- ****/

// Define Vertices and Connectivity of Voronoi Roadmap:
#define NUM_VERTICES 16
// Vertices in ThetaA-ThetaB Space, units deg:
float rad = 180.0 / PI;
float vrm[][] = {
{-1.4792*rad,   -3.0412*rad},
{-0.6414*rad,   -2.6879*rad},
{-0.0742*rad,   -2.2209*rad},
{-0.6746*rad,   -1.1179*rad},
{-1.1126*rad,   -0.0654*rad},
{-1.2270*rad,    1.2086*rad},
{-0.7199*rad,    2.2820*rad},
{-1.3395*rad,    2.9016*rad},
{0.0663*rad,    2.2131*rad},
{0.6065*rad,    2.6651*rad},
{1.2182*rad,    2.9610*rad},
{0.6737*rad,    1.1179*rad},
{1.0900*rad,    0.0620*rad},
{1.2322*rad,   -1.1650*rad},
{0.7025*rad,  -2.2779*rad},
{1.4006*rad, -2.9627*rad}
};
// Connectivity (Adjacency) Matrix of Roadmap Vertices (down is from, across is to):
float MC[][] = {
  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0},
  {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,0,1,1,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0},
  {0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
};

#define sq(a) ((a)*(a))

// Returns the Index of the Roadmap Vertex Closest to the Given Position (ta,tb)
int findNearestVertex(float ta, float tb){
  int closest = 0; // Index of Closest Index
  float dist = 10^80;

  // Loop through Every Vertex, If Connected to Vertex idx, Check if Closer.
  for(int i=0; i<NUM_VERTICES; i++){
    float d = sqrt(sq(vrm[i][0] - ta) + sq(vrm[i][1] - tb));
    if(d<dist){
      dist = d;
      closest = i;
    }
  }

 return closest;
} // #findNearestVertex

// Returns the Index of the Roadmap Vertex Adjacent to the Given Vertex with
// Index idx which is Closest to the Given Target Position, (ta, tb).
int findBestAdjacentVertex(int idx, float ta, float tb){
  int closest = idx; // Index of Closest Index
  float dist = sqrt(sq(vrm[idx][0] - ta) + sq(vrm[idx][1] - tb));

  // Loop through Every Vertex, If Connected to Vertex idx, Check if Closer.
  for(int i=0; i<NUM_VERTICES; i++){
    if(MC[idx][i]){
      float d = sqrt(sq(vrm[i][0] - ta) + sq(vrm[i][1] - tb));
      dist = (d<dist) ? d : dist;
      closest = idx;
    }
  }

 return closest;
} // #findBestAdjacentVertex

// Simple (quick) Depth First Search of Roadmap Vertices for Path from the
// Given Start Vertex with Index, is, to the Given Target Vertex, it.
// Stores the result in the given array, path.
int* DFS(int is, int it, int* path){
  path[0] = is; // Path Starts with Starting Vertex
  int path_length = 1; // Number of Vertices in Path
  int visited[NUM_VERTICES]; // List of Vertices Visited in Search
  int num_visited = 0; // Number of Vertices Visited

  int no_path = 0; // Whether No Path is Possible (Should Never Happen for Real
                  //  (complete, continuous, fully-connected) Roadmap).
  while(path[path_length-1] != it && !no_path){
    // Loop through Every Vertex, If Connected to Current Position and Not Visited,
    // Visit.
    int no_visit = 1; // Whether any No Vertices were Visited (needs back-track)
    for(int i=0; i<NUM_VERTICES; i++){
      if(MC[path[path_length-1]][i]){
        int vis = 0; // Whether this Vertex has been Visited Yet.
        int j=0;
        while(j<num_visited && !vis){ // Allow Early Termination
          vis = (i==visited[j]);
         j++;
        }

        if(!vis){
          // Visit Node, add to path (preliminarily):
          path[path_length] = i;
          path_length++;
          visited[num_visited] = i;
          num_visited++;
          no_visit = 0;
          break;
        } // !vis?
      }
    }
    if(no_visit){
      // No Visitable Nodes Adjacent to this Node. Backtrack.
      path_length--;
      if(path_length < 0){ // No Nodes remain Visitable from Starting Node.
                          //  All Branches Explored. No Path Possible.
        no_path = 1;
      } // path_length?
    } // no_visit?
  } // loop: path

  if(no_path){
    path[0] = -1; // Indicate No Path was Found
  }

  return path;
} // #DFS

/**** -- KINEMATICS -- ****/

#define sgn(x) ((x) / abs(x))

// Geometry:
#define L1 3.79
#define L2 2.52

// Computes the SCARA IK to returns an Array representing the Arm Configuration
// Necessary for the End-Effector to be Positioned at the Given Position (x,y).
// Results in Deg.
float* computeIK(float x, float y){
  float cfg[2];
  cfg[1] = acos( (sq(x) + sq(y) - sq(L1) - sq(L2)) / (2.0 * L1 * L2) );
  cfg[0] = atan2(y,x) - asin( (L2 * sin(cfg[1])) / sqrt(sq(x) + sq(y)) ) - PI/2.0;
  cfg[0] = cfg[0] * 180.0 / PI; // Convert to Deg.
  cfg[1] = cfg[1] * 180.0 / PI;

 return cfg;
} // #computeIK

float init_pos_TH[] = {-90.0, 0.0};

//Ratio of Teeth on Axis of a Joint to that of the Motor.
#define GEAR_RATIO 5

// Returns the Estimated Angle of Joint A
float JointAngleA(){
  return nMotorEncoder[JointA] / GEAR_RATIO + init_pos_TH[0];
} // #JointAngleA
// Returns the Estimated Angle of Joint A
float JointAngleB(){
  return nMotorEncoder[JointB] / GEAR_RATIO + init_pos_TH[1];
} // #JointAngleB

// Target Net Translation Speed through ThetaA-ThetaB Space:
#define TRANSLATION_SPEED (10.0 * GEAR_RATIO)
#define Kp 0.01
// Go To the Given Arm Position / Configuration in ThetaA-ThetaB Space.
void goTo_ang(float tha, float thb){
  static float enca, encb, errA, errB, sea0, seb0, sfa, sfb;
  enca = tha * GEAR_RATIO;
  encb = ((tha-init_pos_TH[0]) + thb) * GEAR_RATIO; // <- requirement of belt-drive (as opposed to direct)

  errA = enca - JointAngleA() * GEAR_RATIO;
  sea0 = sgn(errA); // Original Sign of Error
  errB = encb - JointAngleB() * GEAR_RATIO;
  seb0 = sgn(errB); // Original Sign of Error

  if(abs(errA) > abs(errB)){ // Compute Speed Factors to Ensure both Joint Complete Motion Simultaneously
    sfa = 1.0;
    sfb = abs(errB / errA);
  } else{
    sfb = 1.0;
    sfa = abs(errA / errB);
  }

  while(sgn(errA)*sea0 == 1 || sgn(errB)*seb0 == 1){
    errA = enca - JointAngleA() * GEAR_RATIO;
    errB = encb - JointAngleB() * GEAR_RATIO;
    float spd = TRANSLATION_SPEED;

    float phi = atan2(encb, enca); // Direction of Travel through ThetaA-ThetaB Space
    motor[JointA] = ((float) spd * sfa * sgn(errA));
    motor[JointB] = ((float) spd * sfb * sgn(errB));

    if(sgn(errA)*sea0 != 1){
      motor[JointA] = 0; // stop
    }
    if(sgn(errB)*seb0 != 1){
      motor[JointB] = 0;
    }
  }
    /*
  float eps = 1.25; // Allowable Angle Tolerance (resolution of motor)
  while(abs(errA) > eps || abs(errB) > eps){
    errA = enca - JointAngleA();
    errB = encb - JointAngleB();
    float err = sqrt( sq(errA) + sq(errB) );
    float spd = TRANSLATION_SPEED * Kp * err;

    float phi = atan2(enca, encb); // Direction of Travel through ThetaA-ThetaB Space
    motor[JointA] = ((float) spd * sgn(errA));
    motor[JointB] = ((float) spd * sgn(errB));
  }*/

  motor[JointA] = 0; // stop
  motor[JointB] = 0;
} // #goTo_ang

/**** -- OPERATIONS -- ****/

void init(){
  // Initialize Motors:
  bFloatDuringInactiveMotorPWM = false;

  nMotorPIDSpeedCtrl[JointA] = mtrSpeedReg;
  nMotorPIDSpeedCtrl[JointB] = mtrSpeedReg;
  nMaxRegulatedSpeedNxt = 700;
  nPidUpdateInterval = 2;

  bMotorReflected[JointA] = false;
  bMotorReflected[JointB] = false;

  motor[JointA] = 0;
  motor[JointB] = 0;

  nMotorEncoder[JointA] = 0;
  nMotorEncoder[JointB] = 0;
} // #init

float TA, TB;

int idx_start, idx_targ;
int path[NUM_VERTICES]; // <- initialize (path will necessarily not contain
                       //  more stops than number of vertices.)

task main(){

  init();

  for(int i=0; i<NUM_TARGETS; i++){
    // Get Target:
    float* targ_AB = computeIK(targs[i][0], targs[i][1]); // Target Position in ThA-ThB Space
    TA = targ_AB[0]; TB = targ_AB[1]; // <- For some reason RobotC requires this

    // Get Nodes for Path through Roadmap:
    idx_start = findNearestVertex(JointAngleA(), JointAngleB());
    idx_targ = findNearestVertex(TA, TB);

    //goTo_ang(TA, TB);
    //wait1Msec(1800);

    // Find Path Through Roadmap
    DFS(idx_start,idx_targ, path);

    if(path[0] != -1){ // Path was Found
      // Go To Each Position in the Roadmap Path. End on Target.
      int idx = -1; // Index of Current Vertex
      int j=0;
      while(idx != idx_targ){
        idx = path[j];
        goTo_ang(vrm[idx][0], vrm[idx][1]);
       j++;
      }
      goTo_ang(TA, TB);
      playTone(600,10);
      wait1Msec(3000);
    } // path[0]?

  } // loop: targets
} // #main
