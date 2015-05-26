// 
// 
// 

#include "key_frames.h"
#include "hermite_spline.h"

namespace globalKF{
	int something = 3;
}

// Default constructor
KeyFrames::KeyFrames(){
	m_fn = NULL;
	m_dn = NULL;
	m_fn_recieved = 0;
	m_dn_recieved = 0;
}

// Default destructor
KeyFrames::~KeyFrames(){
	freeMemory();
}

// Initialize static class variables
const int	KeyFrames::G_VALIDATION_PNT_COUNT = 1000;
int			KeyFrames::g_cur_axis = 0;
bool		KeyFrames::g_receiving = false;
int			KeyFrames::g_xn_recieved = 0;
float*		KeyFrames::g_xn = NULL;
int			KeyFrames::g_update_rate = 10;
int			KeyFrames::g_kf_count = 0;
bool		KeyFrames::g_mem_allocted = false;
KeyFrames*	KeyFrames::g_axis_array = NULL;
int			KeyFrames::g_axis_count = 0;
float		KeyFrames::g_max_accel = 20000;
float		KeyFrames::g_max_vel = 4000;


/*** Static Functions ***/

// Points the axis_array var to an existing array of key frames objects that represent the axes to be managed
void KeyFrames::setAxisArray(KeyFrames* p_axis_array, int p_axis_count){
	g_axis_array = p_axis_array;
	g_axis_count = p_axis_count;
}

// Selects the the current axis
void KeyFrames::axis(int p_axis){
	g_cur_axis = p_axis;
}

// Returns the currently selected axis	
int KeyFrames::axis(){
	return g_cur_axis;
}

// Sets the velocity update rate in ms used at run-time
void KeyFrames::updateRate(int p_updates_per_sec){
	g_update_rate = p_updates_per_sec;
}

// Returns the velocity update rate in ms
int KeyFrames::updateRate(){
	return g_update_rate;
}

// Set whether the NMX is currently receiving key frame input data
void KeyFrames::receiveState(bool p_state){
	g_receiving = p_state;
}

// Returns whether the NMX is currently receiving key frame input data
bool KeyFrames::receiveState(){
	return g_receiving;
}

// Sets the key frame count and allocates memory for input vars
void KeyFrames::setKFCount(int p_kf_count){

	if (p_kf_count < 2)
		return;

	g_kf_count = p_kf_count;

	// Free any memory that may have been already allocated
	for (byte i = 0; i < g_axis_count; i++){
		g_axis_array[i].freeMemory();
	}

	// Allocate memory for key frame abscissas
	g_xn = (float *)malloc(g_kf_count * sizeof(float));
	// Reset the number of key frames abscissas assigned
	g_xn_recieved = 0;

	// Allocate memory for the position of each of the axes and reset the received values for each object
	for (byte i = 0; i < g_axis_count; i++){
		g_axis_array[i].m_fn = (float *)malloc(g_kf_count * sizeof(float));
		g_axis_array[i].m_dn = (float *)malloc(g_kf_count * sizeof(float));
		g_axis_array[i].m_fn_recieved = 0;
		g_axis_array[i].m_dn_recieved = 0;
	}
	g_mem_allocted = true;
}

// Returns the key frame count
int KeyFrames::countKF(){
	return g_kf_count;
}

// Points xn to an existing array of values
void KeyFrames::setXN(float* p_xn){

	if (!g_mem_allocted)
		return;

	g_xn = p_xn;
}

// Assigns xn values one at a time	
void KeyFrames::setXN(float p_input){
	g_xn[g_xn_recieved] = p_input;
	g_xn_recieved++;
}

// Returns the number of xn values that have been assigned. Accurate only when assigning values one at a time.
int KeyFrames::countXN(){
	return g_xn_recieved;
}

// Resets the xn received count
void KeyFrames::resetXN(){
	g_xn_recieved = 0;
}

// Returns the abscissa of the requested key frame
float KeyFrames::getXN(int p_which){
	return g_xn[p_which];
}


/*** Non-Static Functions ***/

// Deallocates any memory assigned to input arrays
void KeyFrames::freeMemory(){
	// If the count has been previously set, deallocate the memory from last time
	if (g_mem_allocted){
		free(g_xn);
		for (byte i = 0; i < g_axis_count; i++){
			free(g_axis_array[i].m_fn);
			free(g_axis_array[i].m_dn);
		}
		g_mem_allocted = false;
	}
}

void KeyFrames::setFN(float* p_fn){
	if (!g_mem_allocted)
		return;
	m_fn = p_fn;
}

void KeyFrames::setFN(float p_input){
	if (!g_mem_allocted)
		return;
	m_fn[m_fn_recieved] = p_input;	
	m_fn_recieved++;
}

int KeyFrames::countFN(){
	return m_fn_recieved;
}

// Resets the fn received count
void KeyFrames::resetFN(){
	m_fn_recieved= 0;
}

float KeyFrames::getFN(int p_which){
	return m_fn[m_fn_recieved];
	m_fn_recieved++;
}

void KeyFrames::setDN(float* p_dn){
	if (!g_mem_allocted)
		return;
	m_dn = p_dn;
}

void KeyFrames::setDN(float p_input){
	if (!g_mem_allocted)
		return; 
	m_dn[m_dn_recieved] = p_input;
	m_dn_recieved++;
}

int KeyFrames::countDN(){
	return m_dn_recieved;
}

// Resets the fn received count
void KeyFrames::resetDN(){
	m_dn_recieved = 0;
}

float KeyFrames::getDN(int p_which){
	return m_dn[p_which];
}

float KeyFrames::pos(float p_x){
	updateVals(p_x);
	return m_f[0];
}

float KeyFrames::vel(float p_x){
	updateVals(p_x);
	return m_d[0];
}

float KeyFrames::accel(float p_x){
	updateVals(p_x);
	return m_s[0];
}

bool KeyFrames::validateVel(){

	float increment = g_xn[g_kf_count - 1] / G_VALIDATION_PNT_COUNT - 1;	
	
	for (int i = 0; i < G_VALIDATION_PNT_COUNT; i++){
		updateVals(i * increment);
		if (abs(m_d[0]) > g_max_vel)
			return false;
	}
	return true;
}

bool KeyFrames::validateAccel(){

	float increment = g_xn[g_kf_count - 1] / G_VALIDATION_PNT_COUNT - 1;

	for (int i = 0; i < G_VALIDATION_PNT_COUNT; i++){
		updateVals(i * increment);
		if (abs(m_s[0]) > g_max_accel)
			return false;
	}
	return true;
}

void KeyFrames::setMaxVel(float p_max_vel){
	g_max_vel = p_max_vel;
}

void KeyFrames::setMaxAccel(float p_max_accel){
	g_max_accel = p_max_accel;
}

/*** Non-Static Private Functions ***/

void KeyFrames::updateVals(float p_x){
	float x_point[1] = { p_x };
	HermiteSpline::cubic_spline_value(g_kf_count, g_xn, m_fn, m_dn, 1, x_point, m_f, m_d, m_s);
}