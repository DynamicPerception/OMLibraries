#include "CubicBezier.h"

/**************************************/
/*									  */
/*         OrderedPair Class		  */
/*									  */
/**************************************/

#pragma region OrderedPair Class Functions

OrderedPair::OrderedPair(){}

OrderedPair::OrderedPair(float p_x, float p_y){	
	m_vals[0] = p_x;
	m_vals[1] = p_y;
}

float OrderedPair::val(int p_which){
	// Enforce a valid request
	p_which = p_which < 0 ? 0 : p_which > 1 ? 1 : p_which;
	return m_vals[p_which];
}

void OrderedPair::val(int p_which, float p_val){	
	m_vals[p_which] = p_val;
}

float OrderedPair::x(){
	return m_vals[0];
}

void OrderedPair::x(int p_x){
	m_vals[0] = p_x;
}

float OrderedPair::y(){
	return m_vals[1];
}

void OrderedPair::y(int p_y){
	m_vals[1] = p_y;
}


#pragma region Arithmatic Operator Overrides
OrderedPair operator * (int p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator * (const OrderedPair& p_op, int p_x){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator * (float p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator * (const OrderedPair& p_op, float p_x){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator * (double p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator * (const OrderedPair& p_op, double p_x){
	return OrderedPair(p_op.m_vals[0] * p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator + (int p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] + p_x, p_op.m_vals[1] + p_x);
}
OrderedPair operator + (float p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] + p_x, p_op.m_vals[1] + p_x);
}
OrderedPair operator + (const OrderedPair& p_op, int p_x){
	return OrderedPair(p_op.m_vals[0] + p_x, p_op.m_vals[1] + p_x);
}
OrderedPair operator + (const OrderedPair& p_op, float p_x){
	return OrderedPair(p_op.m_vals[0] + p_x, p_op.m_vals[1] + p_x);
}
OrderedPair operator + (const OrderedPair& p_op1, const OrderedPair& p_op2){
	return OrderedPair(p_op1.m_vals[0] + p_op2.m_vals[0], p_op1.m_vals[1] + p_op2.m_vals[1]);
}
OrderedPair operator - (int p_x, const OrderedPair& p_op){
	return OrderedPair(p_op.m_vals[0] - p_x, p_op.m_vals[1] * p_x);
}
OrderedPair operator - (const OrderedPair& p_op){
	return OrderedPair(-p_op.m_vals[0], -p_op.m_vals[1]);
}
#pragma endregion

#pragma endregion

/**************************************/
/*									  */
/*			  Span Class			  */
/*									  */
/**************************************/

#pragma region Span Class

// Constructors
#pragma region Span Constructors
Span::Span(){}

Span::Span(OrderedPair *p_ctrl_pts, Span *p_prev_span){		
	m_id = g_id_gen++;	
	for (int i = 0; i < g_MAX_CTRL_PTS; i++){
		m_ctrl_pts[i] = p_ctrl_pts[i];
	}	
	m_next_span = NULL;
	prevSpan(p_prev_span);
	setCoeffs();
}
#pragma endregion Span Constructors

/********** Public Members **********/

// Meta functions
#pragma region Span Meta Functions
int Span::id(){
	return m_id;
}

void Span::nextSpan(Span *p_next_span){	
	m_next_span = p_next_span;
}

Span* Span::nextSpan(){
	return m_next_span;
}

void Span::prevSpan(Span *p_prev_span){	
	m_prev_span = p_prev_span;	
	if (m_prev_span != NULL)
		m_prev_span->nextSpan(this);
}

Span* Span::prevSpan(){
	return m_prev_span;
}
#pragma endregion Span Meta Functions

// Control points getter
#pragma region Control points getter
OrderedPair *Span::ctrlPts(){
	return m_ctrl_pts;
}
#pragma endregion Control points getter

// Evaluation w/r/t X
#pragma region Evaluation w/r/t X

float Span::positionAtX(float p_x){	
	float t = tOfX(p_x, 0.5, 15);	
	return solveCubic(t, false);	
};

float Span::velocityAtX(float p_x){
	float t = tOfX(p_x, 0.5, 15);
	return solveCubicPrime(t, false);
};

float Span::accelAtX(float p_x){
    float t = tOfX(p_x, 0.5, 15);
    return solveCubicDoublePrime(t, false);
};

#pragma endregion Evaluation w/r/t X


/********** Private Members **********/

// Static vars
#pragma region Static vars
int Span::g_id_gen = 0;
#pragma endregion Static vars

// Defining functions
#pragma region Defining functions
void Span::setCoeffs(){
	// Calculate the coefficients
	m_coeff_A = (-m_ctrl_pts[0]) + (3 * m_ctrl_pts[1]) + (-3 * m_ctrl_pts[2]) + m_ctrl_pts[3];
	m_coeff_B = (3 * m_ctrl_pts[0]) + (-6 * m_ctrl_pts[1]) + (3 * m_ctrl_pts[2]);
	m_coeff_C = (-3 * m_ctrl_pts[0]) + (3 * m_ctrl_pts[1]);
	m_coeff_D = m_ctrl_pts[0];	
}
#pragma endregion Defining functions

// Cubic solvers
#pragma region Cubic solvers
float Span::solveCubic(float p_val, bool p_is_x){
	solveCubic(p_val, p_is_x, 0);
}
/**
	@param p_val Value for which the equation will be evaluated
	@param p_x_coeffs Should the set of x coefficients be used? If not, y coefficients will be used instead.
	@param p_offset For use when finding cubic root. The x/y value for which parameter t is sought. It is
		subtracted from the final value such that the result is a solution to 
		the cubic in standard form At^3 + Bt^2 + Ct + (D - p_offset) = 0
*/
float Span::solveCubic(float p_val, bool p_x_coeffs, float p_offset){	
	float ret;
	int which = p_x_coeffs ? 0 : 1;
	// P(t) = At^3 + Bt^2 + Ct + D
	// Calculate via Horner's rule	
	float term = p_val;
	ret = m_coeff_D.val(which) - p_offset;
	ret += term * m_coeff_C.val(which);
	term *= p_val;
	ret += term * m_coeff_B.val(which);
	term *= p_val;
	ret += term * m_coeff_A.val(which);
	/* 
		No idea why, but without this NOP, the return 
		value doesn't always "stick" in variables of calling
		functions.
	*/
	__asm__("nop");
	return ret;
}

float Span::solveCubicPrime(float p_val, bool p_is_x){
	float ret;
	int which = p_is_x ? 0 : 1;
	// P'(t) = 3At^2 + 2Bt + C
	// Calculate via Horner's rule
	float term = p_val;	
	ret = m_coeff_C.val(which);			
	ret += term * m_coeff_B.val(which) * 2;	
	term *= p_val;
	ret += term * m_coeff_A.val(which) * 3;		
	return ret;
}

float Span::solveCubicDoublePrime(float p_val, bool p_is_x){
    float ret;
    int which = p_is_x ? 0 : 1;
    // P'(t) = 6At + 2B    
    float term = p_val;
    ret = 6 * m_coeff_A.val(which) * term + 2* m_coeff_B.val(which);    
    return ret;
}

#pragma endregion Cubic solvers

// Evaluation w/r/t X
#pragma region Evaluation w/r/t X
/**
	This function determines the t value along the span for
	a given value of x (in this case, our time variable) using
	Newton's method.

	@param p_x The Bezier x value for which to solve for parameter t
	@param p_guess Initial guess of t value
	@param p_recursion_limit Maximum number of iterations of Newton's method
		to run before returning the result of a non-converged solution

*/
float Span::tOfX(float p_x, float p_guess, int p_recursion_limit){
	if (p_x == 0)
		return 0.0;

	static int recurse_index = 0;
	float convergence_threshold = 1.5e-5;

	// Find f(guess) and f'(guess)
	float f_of_guess = solveCubic(p_guess, true, p_x);	
	float f_prime_of_guess = solveCubicPrime(p_guess, true);
	// Newton's method
	float new_guess = p_guess - (f_of_guess / f_prime_of_guess);
	recurse_index++;
	// Return when close enough or exceeding recursion limit
	if (abs(new_guess - p_guess) < convergence_threshold || recurse_index == p_recursion_limit){
		// Be sure to reset the resursion index, otherwise the function 
		// will return after too few iterations upon future calls
		recurse_index = 0;
		return new_guess;
	}
	// Otherwise do another iteration
	else{
		return tOfX(p_x, new_guess, p_recursion_limit);
	}
}
#pragma endregion Evaluation w/r/t X

// **** Helper Functions *** //

#pragma region Helper Functions
bool Span::containsX(float p_x){	
	if (p_x >= startX() && p_x <= stopX())	
		return true;
	else		
		return false;
}

bool Span::containsY(float p_y){
	if (startY() <= p_y && p_y <= stopY())
		return true;
	else
		return false;
}

float Span::startX(){		
	return m_ctrl_pts[0].x();
}

float Span::stopX(){
	return m_ctrl_pts[g_MAX_CTRL_PTS-1].x();
}

float Span::rangeX(){
    return stopX() - startX();
}

float Span::startY(){
	return m_ctrl_pts[0].y();
}

float Span::stopY(){
	return m_ctrl_pts[g_MAX_CTRL_PTS-1].y();
}

float Span::rangeY(){
    return stopY() - startY();
}
#pragma endregion Helper Functions

#pragma endregion Span Class

/**************************************/
/*									  */
/*         CubicBezier Class		  */
/*									  */
/**************************************/

#pragma region CubicBezier Class

CubicBezier::CubicBezier(){
	init(NULL, 2);    
    m_next_x = 0;
    m_next_y = 0;
}


CubicBezier::CubicBezier(OrderedPair *p_ctrl_pts, int p_knot_count){
	// Must have at least 2 knots to define a spline	
	if (p_knot_count < 2)
		return;	
	init(p_ctrl_pts, p_knot_count);
    // It is assumed that the correct number of control
    // points have been passed to the constructor
    m_next_x = m_ctrl_pt_count;
    m_next_y = m_ctrl_pt_count;
}

void CubicBezier::init(OrderedPair *p_ctrl_pts, int p_knot_count){
	m_ctrl_pts = p_ctrl_pts;
	m_knot_count = p_knot_count;
	m_span_count = p_knot_count - 1;
	m_span_mem_allocated = false;
	m_ctrl_mem_allocated = false;	
}

// Default destructor
CubicBezier::~CubicBezier(){
	releaseMemory();
}

/**
    @param p_count The number of knots that will compose the CubicBezier objects
    @return The number of control points needed to define this CubicBezier object
*/
int CubicBezier::knotCount(int p_count){
	m_knot_count = p_count;
	m_span_count = m_knot_count - 1;		
	/*
		Allocate memory for control points here rather
		than pointing to an external point array
	*/
    m_ctrl_pt_count = m_span_count * (g_PTS_PER_SPAN - 1) + 1;	// span n pt[3] == span n+1 pt[0], so no need to duplicate linking points
	releaseMemory();
    m_ctrl_pts = (OrderedPair *)malloc(m_ctrl_pt_count * sizeof(OrderedPair));
    for (int i = 0; i < m_ctrl_pt_count; i++){
		m_ctrl_pts[i] = OrderedPair();
	}

	m_next_x = 0;
	m_next_y = 0;

    return m_ctrl_pt_count;
}

int CubicBezier::knotCount(){
	return m_knot_count;
}

int CubicBezier::controlPointCount(){
    return m_ctrl_pt_count;
}

void CubicBezier::releaseMemory(){
	if (m_span_mem_allocated){
		free(m_spans);
		m_span_mem_allocated = false;
	}
	if (m_ctrl_mem_allocated){
		free(m_ctrl_pts);
		m_ctrl_mem_allocated = false;
	}
}

void CubicBezier::setNextX(float p_x){
	OrderedPair op = m_ctrl_pts[m_next_x];
	op.x(p_x);
	m_ctrl_pts[m_next_x] = op;
	m_next_x++;
    // This only actually initializes once all the points have been set
    initSpans();
}

void CubicBezier::setNextY(float p_y){
	OrderedPair op = m_ctrl_pts[m_next_y];
	op.y(p_y);
	m_ctrl_pts[m_next_y] = op;
	m_next_y++;
    // This only actually initializes once all the points have been set
    initSpans();
}

float CubicBezier::getXN(int p_kf){
    int pnt = p_kf * g_PTS_PER_SPAN;
    return m_ctrl_pts[pnt].x();
}

float CubicBezier::getFN(int p_kf){
    int pnt = p_kf * g_PTS_PER_SPAN;
    return m_ctrl_pts[pnt].y();
}

void CubicBezier::initSpans(){
		
    // Don't initialize the spans unless the control points have been fully set
    if (!(m_next_x == m_ctrl_pt_count && m_next_y == m_ctrl_pt_count)){
        return;
    }

	// Dynamically allocate memory for span array
	if (m_span_mem_allocated){
		free(m_spans);
	}
	size_t span_size = sizeof(Span);		
	size_t memory_needed = m_span_count * span_size;	
	m_spans = (Span *)malloc(memory_needed);
	m_span_mem_allocated = true;

	/** Select starting control point for each span and create new span objects **/

	int PT_CT = 4;			// Number of control points that define a span
	int INC = PT_CT - 1;	// Number of points to increment when defining new span (last point of span n should == first point of span n+1)
	for (int i = 0; i < m_span_count; i++){				
		// Don't pass a garbage address to the pointer parameter for the first span
		Span* prev_span = i == 0 ? NULL : &m_spans[i - 1];		
		m_spans[i] = Span((m_ctrl_pts + i * INC), prev_span);		
	}

	// Set current span to first available
	m_cur_span = &m_spans[0];			
}

float CubicBezier::positionAtX(float p_x){	
    Span *this_span = spanContainingX(p_x);
    if (this_span != NULL)
        return this_span->positionAtX(p_x);
    else
        return g_ERROR;
}

float CubicBezier::velocityAtX(float p_x){    
    Span *this_span = spanContainingX(p_x);
    if (this_span != NULL)
        return this_span->velocityAtX(p_x);
    else
        return g_ERROR;
}

float CubicBezier::accelAtX(float p_x){
    Span *this_span = spanContainingX(p_x);
    if (this_span != NULL)
        return this_span->accelAtX(p_x);
    else
        return g_ERROR;
}

Span *CubicBezier::spanContainingX(float p_x){
    for (int i = 0; i < m_span_count; i++){
        if (m_spans[i].containsX(p_x)){
            return &m_spans[i];
        }
    }
    return NULL;
}

float CubicBezier::startX(){
    if (m_spans != NULL)
        return m_spans[0].startX();
    else
        return 0.0;
}

float CubicBezier::stopX(){
    if (m_spans != NULL)
        return m_spans[m_span_count - 1].stopX();
    else
        return 0.0;
}

float CubicBezier::startY(){
    if (m_spans != NULL)
        return m_spans[0].startY();
    else
        return 0.0;
}

float CubicBezier::stopY(){
    if (m_spans != NULL)
        return m_spans[m_span_count - 1].stopX();
    else
        return 0.0;
}

Span *CubicBezier::getSpan(int p_which){
	return &m_spans[p_which];
}

#pragma endregion CubicBezier Class
