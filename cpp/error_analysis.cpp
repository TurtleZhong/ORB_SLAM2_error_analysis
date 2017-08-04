#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <limits>
#include <sstream>

#include "mail.h"
#include "matrix.h"

using namespace std;

int step_frame = 3;

struct errors {
  int32_t current_frame;
  int32_t reference_frame;
  float   r_err;
  float   t_err;
  float   dist;   //distance between current_frame and reference_frame
  float   speed;
  errors (int32_t current_frame,int32_t reference_frame,float r_err,float t_err,float dist,float speed) :
    current_frame(current_frame),reference_frame(reference_frame),r_err(r_err),t_err(t_err),dist(dist),speed(speed) {}
};


vector<Matrix> loadPoses(string file_name) {
  vector<Matrix> poses;
  FILE *fp = fopen(file_name.c_str(),"r");
  if (!fp)
    return poses;
  while (!feof(fp)) {
    Matrix P = Matrix::eye(4);
    if (fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                   &P.val[0][0], &P.val[0][1], &P.val[0][2], &P.val[0][3],
                   &P.val[1][0], &P.val[1][1], &P.val[1][2], &P.val[1][3],
                   &P.val[2][0], &P.val[2][1], &P.val[2][2], &P.val[2][3] )==12) {
      poses.push_back(P);
    }
  }
  fclose(fp);
  return poses;
}

vector<float> trajectoryDistances (vector<Matrix> &poses) {
  vector<float> dist;
  dist.push_back(0);
  for (int32_t i=1; i<poses.size(); i++) {
    Matrix P1 = poses[i-1];
    Matrix P2 = poses[i];
    float dx = P1.val[0][3]-P2.val[0][3];
    float dy = P1.val[1][3]-P2.val[1][3];
    float dz = P1.val[2][3]-P2.val[2][3];
    dist.push_back(dist[i-1]+sqrt(dx*dx+dy*dy+dz*dz));
  }
  return dist;
}

inline float rotationError(Matrix &pose_error) {
  float a = pose_error.val[0][0];
  float b = pose_error.val[1][1];
  float c = pose_error.val[2][2];
  float d = 0.5*(a+b+c-1.0);
  return acos(max(min(d,1.0f),-1.0f));
}

inline float translationError(Matrix &pose_error) {
  float dx = pose_error.val[0][3];
  float dy = pose_error.val[1][3];
  float dz = pose_error.val[2][3];
  return sqrt(dx*dx+dy*dy+dz*dz);
}


vector<errors> calcSequenceErrors (vector<Matrix> &poses_gt,vector<Matrix> &poses_result)
{
    //error vector
    vector<errors> err;

    //here we set the step_frame as 3 (the initial value)

    vector<float> dist = trajectoryDistances(poses_gt);

    //for all frames we do
    //here we choose the 10th frame as the first one because we need to
    //let the reference frame not be zero

    for (int32_t current_frame = 10; current_frame < poses_gt.size(); current_frame++)
    {

        //here we know that the step_frame = 3(initial)
        int32_t reference_frame = current_frame - step_frame;

        // compute rotational and translational errors

        Matrix pose_delta_gt     = Matrix::inv(poses_gt[reference_frame])*poses_gt[current_frame];
        Matrix pose_delta_result = Matrix::inv(poses_result[reference_frame])*poses_result[current_frame];
        Matrix pose_error        = Matrix::inv(pose_delta_result)*pose_delta_gt;
        float r_err = rotationError(pose_error);
        float t_err = translationError(pose_error);
        float distance = dist[current_frame] - dist[reference_frame];

        //compute speed
        float num_frames = (float)(current_frame-reference_frame+1);
        float speed = distance/(0.1*num_frames);

        err.push_back(errors(current_frame,reference_frame,r_err/distance,t_err/distance,distance,speed));

    }

    return err;

}


void saveSequenceErrors (vector<errors> &err,string file_name) {

  // open file
  FILE *fp;
  fp = fopen(file_name.c_str(),"w");

  // write to file
  for (vector<errors>::iterator it=err.begin(); it!=err.end(); it++)
    fprintf(fp,"%d %d %f %f %f %f\n",it->current_frame,it->reference_frame,it->r_err,it->t_err,it->dist,it->speed);

  // close file
  fclose(fp);
}


bool eval (string result_sha,Mail* mail) {

  // ground truth and result directories
  string gt_dir         = "data/odometry/poses";
  string result_dir     = "results/" + result_sha;

  stringstream ss;
  string tmp;
  ss << step_frame;
  ss >> tmp;


  string error_analysis = result_dir + "/error_analysis" + "/error_analysis_" + tmp;

  // cout the path

  cout << "gt_dir path is: " << gt_dir << endl;
  cout << "result_dir is: " << result_dir << endl;




  // create output directories
  system(("mkdir " + error_analysis).c_str());

  // for all sequences do
  for (int32_t i=11; i<22; i++) {

    // file name
    char file_name[256];
    sprintf(file_name,"%02d.txt",i);
    cout << "file name is :" << file_name << endl;

    cout << "txt_gt path is: " << gt_dir + "/" + file_name << endl;
    cout << "txt_orb path is: " << result_dir + "/data/" + file_name << endl;

    // read ground truth and result poses
    vector<Matrix> poses_gt     = loadPoses(gt_dir + "/" + file_name);
    vector<Matrix> poses_result = loadPoses(result_dir + "/data/" + file_name);


    // plot status
    mail->msg("Processing: %s, poses: %d/%d",file_name,poses_result.size(),poses_gt.size());

    // check for errors
    if (poses_gt.size()==0 || poses_result.size()!=poses_gt.size()) {
      mail->msg("ERROR: Couldn't read (all) poses of: %s", file_name);
      return false;
    }

    // compute sequence errors
    vector<errors> seq_err = calcSequenceErrors(poses_gt,poses_result);
    saveSequenceErrors(seq_err,error_analysis + "/" + file_name);

  }

  // success
    return true;
}


int32_t main (int32_t argc,char *argv[]) {

  // we need 2 or3 or  4 arguments!
  if (argc!=2 && argc!=4 && argc!=3) {
    cout << "Usage: ./eval_odometry result_sha step_frame [user_sha email]" << endl;
    return 1;
  }

  // read arguments
  string result_sha = argv[1];
  stringstream tmp;
  tmp << argv[2];
  tmp >> step_frame;

  // init notification mail
  Mail *mail;
  if (argc==4) mail = new Mail(argv[3]);
  else         mail = new Mail();
  mail->msg("Thank you for participating in our evaluation!");

  // run evaluation
  bool success = eval(result_sha,mail);
  if (argc==4) mail->finalize(success,"odometry",result_sha,argv[2]);
  else         mail->finalize(success,"odometry",result_sha);

  // send mail and exit
  delete mail;
  return 0;
}
