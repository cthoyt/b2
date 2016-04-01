//This file is part of Bertini 2.0.
//
//function_tree_test.cpp is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//function_tree_test.cpp is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with function_tree_test.cpp.  If not, see <http://www.gnu.org/licenses/>.
//

//  function_tree_test.cpp
//
//  Created by Collins, James B. on 4/30/15.
//  Copyright (c) 2015 West Texas A&M University. All rights reserved.
//
// also modified by
//  Daniel Brake
//  University of Notre Dame
//  ACMS
//  Spring, Summer 2015

#include <iostream>

#include <cstdlib>
#include <cmath>
#include <vector>

#include "bertini.hpp"
#include "function_tree.hpp"


#include <boost/spirit/include/qi.hpp>
#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>



using Variable = bertini::node::Variable;
using Node = bertini::node::Node;

using Function = bertini::node::Function;
using Jacobian = bertini::node::Jacobian;






unsigned DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS = 30;


// double threshold_clearance_d = 1e-10;
// double threshold_clearance_mp = 1e-25;


extern double threshold_clearance_d;
extern double threshold_clearance_mp;
// these have been commented out, and we will use the ones defined in function_tree_test.
// std::string xstr_real = "3.1";
// std::string xstr_imag = "0.3";
// std::string ystr_real = "8.8";
// std::string ystr_imag = "1.4";
// std::string astr_real = "3.4";
// std::string astr_imag = "5.6";
// std::string bstr_real = "-0.2";
// std::string bstr_imag = "-2.1";
std::string zstr_real = "0.8";
std::string zstr_imag = "-1.7";
extern std::string xstr_real;
extern std::string xstr_imag;

extern std::string ystr_real;
extern std::string ystr_imag;
extern std::string astr_real;
extern std::string astr_imag;
extern std::string bstr_real;
extern std::string bstr_imag;
// extern std::string zstr_real;
// extern std::string zstr_imag;



extern dbl xnum_dbl;
extern dbl ynum_dbl;
extern dbl anum_dbl;
extern dbl bnum_dbl;

// dbl xnum_dbl(std::stod(xstr_real), std::stod(xstr_imag));
// dbl ynum_dbl(std::stod(ystr_real), std::stod(ystr_imag));
// dbl anum_dbl(std::stod(astr_real), std::stod(astr_imag));
// dbl bnum_dbl(std::stod(bstr_real), std::stod(bstr_imag));
dbl znum_dbl(std::stod(zstr_real), std::stod(zstr_imag));

extern mpfr xnum_mpfr;
extern mpfr ynum_mpfr;
extern mpfr anum_mpfr;
extern mpfr bnum_mpfr;

// mpfr xnum_mpfr(xstr_real, xstr_imag);
// mpfr ynum_mpfr(ystr_real, ystr_imag);
// mpfr anum_mpfr(astr_real, astr_imag);
// mpfr bnum_mpfr(bstr_real, bstr_imag);
mpfr znum_mpfr(zstr_real, zstr_imag);

Eigen::Matrix<dbl, 3, 1> var_dbl;
Eigen::Matrix<mpfr, 3, 1> var_mpfr;





BOOST_AUTO_TEST_SUITE(differentiate)

/////////// Basic Operations Alone ///////////////////

BOOST_AUTO_TEST_CASE(just_diff_a_function){
	std::string str = "function f; variable_group x,y,z; f = x*y +y^2 - z*x + 9;";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);
	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());
	for(auto vv : vars)
	{
		JFunc->EvalJ<dbl>(vv);
		JFunc->EvalJ<mpfr>(vv);
	}

	std::vector<int> multidegree{1,2,1};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);
	
	BOOST_CHECK_EQUAL(func->Degree(vars), 2);
}


BOOST_AUTO_TEST_CASE(diff_3xyz){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = 3*x*y*z;";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	BOOST_CHECK_EQUAL(func->Degree(),3);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),1);

	

	std::vector<int> multidegree{1,1,1};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 3);

	std::vector<dbl> exact_dbl = {3.0*ynum_dbl*znum_dbl, 3.0*xnum_dbl*znum_dbl, 3.0*ynum_dbl*xnum_dbl};
	std::vector<mpfr> exact_mpfr = {mpfr("3.0")*ynum_mpfr*znum_mpfr,mpfr("3.0")*xnum_mpfr*znum_mpfr,mpfr("3.0")*ynum_mpfr*xnum_mpfr};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() / exact_dbl[0].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() / exact_dbl[0].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() / exact_mpfr[0].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() / exact_mpfr[0].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() / exact_dbl[1].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() / exact_dbl[1].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() / exact_mpfr[1].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() / exact_mpfr[1].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() / exact_dbl[2].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() / exact_dbl[2].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() / exact_mpfr[2].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() / exact_mpfr[2].imag() -1) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_constant){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = 4.5 + i*8.2;";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{0,0,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 0);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	


	std::vector<dbl> exact_dbl = {0.0, 0.0, 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),mpfr("0.0"),mpfr("0.0")};

	for(int ii = 0; ii < vars.size(); ++ii)
	{
		BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[ii]).real() - exact_dbl[ii].real() ) < threshold_clearance_d);
		BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[ii]).imag() - exact_dbl[ii].imag()) < threshold_clearance_d);
		BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[ii]).real() - exact_mpfr[ii].real() ) < threshold_clearance_mp);
		BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[ii]).imag() - exact_mpfr[ii].imag() ) < threshold_clearance_mp);
	}
}


BOOST_AUTO_TEST_CASE(diff_sum_xyz_constant){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = x-y+z-4.5+i*7.3;";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{1,1,1};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 1);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),1);


	std::vector<dbl> exact_dbl = {1.0, -1.0, 1.0};
	std::vector<mpfr> exact_mpfr = {mpfr("1.0"),mpfr("-1.0"),mpfr("1.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);

}



BOOST_AUTO_TEST_CASE(diff_x_squared_times_z_cubed){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = (x^2)*(y^3);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{2,3,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 5);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),2);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),3);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);


	std::vector<dbl> exact_dbl = {2.0*xnum_dbl*pow(ynum_dbl,3.0), 3.0*pow(ynum_dbl*xnum_dbl,2.0), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("2.0")*xnum_mpfr*pow(ynum_mpfr,3.0),mpfr("3.0")*pow(ynum_mpfr,2)*pow(xnum_mpfr,2.0),mpfr("0.0")};

	

	
	JFunc->Reset();
	sys.SetVariables<dbl>(var_dbl);
	auto J_val_0_d = JFunc->EvalJ<dbl>(vars[0]);

	JFunc->Reset();
	sys.SetVariables<dbl>(var_dbl);
	auto J_val_1_d = JFunc->EvalJ<dbl>(vars[1]);

	JFunc->Reset();
	sys.SetVariables<dbl>(var_dbl);
	auto J_val_2_d = JFunc->EvalJ<dbl>(vars[2]);


	JFunc->Reset();
	sys.SetVariables<mpfr>(var_mpfr);
	auto J_val_0_mp = JFunc->EvalJ<mpfr>(vars[0]);

	JFunc->Reset();
	sys.SetVariables<mpfr>(var_mpfr);
	auto J_val_1_mp = JFunc->EvalJ<mpfr>(vars[1]);

	JFunc->Reset();
	sys.SetVariables<mpfr>(var_mpfr);
	auto J_val_2_mp = JFunc->EvalJ<mpfr>(vars[2]);


	BOOST_CHECK(fabs(J_val_0_d.real() / exact_dbl[0].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_0_d.imag() / exact_dbl[0].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_0_mp.real()/exact_mpfr[0].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J_val_0_mp.imag()/exact_mpfr[0].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(J_val_1_d.real() / exact_dbl[1].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_1_d.imag() / exact_dbl[1].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_1_mp.real()/exact_mpfr[1].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J_val_1_mp.imag()/exact_mpfr[1].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(J_val_2_d.real() - exact_dbl[2].real()) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_2_d.imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(J_val_2_mp.real()-exact_mpfr[2].real()) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J_val_2_mp.imag()-exact_mpfr[2].imag()) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_x_squared_over_y_cubed){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = (x^2)/(y^3);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{2,-1,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),2);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);
	



	std::vector<dbl> exact_dbl = {2.0*xnum_dbl/pow(ynum_dbl,3.0), -3.0*pow(xnum_dbl,2.0)/pow(ynum_dbl,4.0), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("2.0")*xnum_mpfr/pow(ynum_mpfr,3.0),mpfr("-3.0")*pow(xnum_mpfr,2.0)/pow(ynum_mpfr,4.0),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_x_squared_times_lx_plus_numl){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = (x^2)*(x+3);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{3,0,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 3);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),3);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);



	std::vector<dbl> exact_dbl = {3.0*pow(xnum_dbl,2.0) + 6.0*xnum_dbl, 0.0, 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("3.0")*pow(xnum_mpfr,mpfr("2.0")) + mpfr("6.0")*xnum_mpfr,mpfr("0.0"),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() / exact_dbl[0].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() / exact_dbl[0].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() / exact_mpfr[0].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() / exact_mpfr[0].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}

BOOST_AUTO_TEST_CASE(diff_2y_over_ly_squared_minus_numl){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = y/(y+1);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{0,-1,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	

	std::vector<dbl> exact_dbl = {0.0, pow(ynum_dbl+1.0,-2.0), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),pow(ynum_mpfr+mpfr("1.0"),mpfr("-2.0")),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}



BOOST_AUTO_TEST_CASE(diff_sin_x){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = sin(x);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{-1,0,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	
	std::vector<dbl> exact_dbl = {cos(xnum_dbl), 0.0, 0.0};
	std::vector<mpfr> exact_mpfr = {cos(xnum_mpfr),mpfr("0.0"),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_cos_y){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = cos(y);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{0,-1,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	

	std::vector<dbl> exact_dbl = {0.0, -1.0*sin(ynum_dbl), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),-sin(ynum_mpfr),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_tan_z){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = tan(z);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),-1);

   BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	std::vector<dbl> exact_dbl = {0.0,0.0, (1.0/cos(znum_dbl))*(1.0/cos(znum_dbl))};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),mpfr("0.0"),(mpfr("1.0")/cos(znum_mpfr))*(mpfr("1.0")/cos(znum_mpfr))};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_exp_x){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = exp(x);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());


	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);


	std::vector<dbl> exact_dbl = {exp(xnum_dbl), 0.0, 0.0};
	std::vector<mpfr> exact_mpfr = {exp(xnum_mpfr),mpfr("0.0"),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_log_x){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = log(x^2+y);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);
	sys.Differentiate();

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);


	std::vector<dbl> exact_dbl = {2.0*xnum_dbl/(xnum_dbl*xnum_dbl+ynum_dbl), 1.0/(xnum_dbl*xnum_dbl+ynum_dbl), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr(2.0)*xnum_mpfr/(xnum_mpfr*xnum_mpfr+ynum_mpfr), mpfr(1.0)/(xnum_mpfr*xnum_mpfr+ynum_mpfr),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_sqrt_y){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = sqrt(y);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());


	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	
	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	std::vector<dbl> exact_dbl = {0.0, 0.5/sqrt(ynum_dbl), 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),mpfr("0.5")/sqrt(ynum_mpfr),mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}



/////////// Chain Rule ///////////////////
BOOST_AUTO_TEST_CASE(diff_lz_plus_3l_cubed){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = (z+3)^3;";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{0,0,3};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),3);

	
	BOOST_CHECK_EQUAL(func->Degree(vars), 3);

	std::vector<dbl> exact_dbl = {0.0, 0.0, 3.0*(pow(znum_dbl+3.0,2.0))};
	std::vector<mpfr> exact_mpfr = {mpfr("0.0"),mpfr("0.0"),mpfr("3.0")*pow(znum_mpfr+mpfr("3.0"),mpfr("2.0"))};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}




BOOST_AUTO_TEST_CASE(diff_x_squared_plus_y_squared_plus_z_squared){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = x^2+y^2+z^2;";

	bertini::System sys;
	

	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{2,2,2};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars), 2);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),2);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),2);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),2);

	


	std::vector<dbl> exact_dbl = {2.0*xnum_dbl, 2.0*ynum_dbl, 2.0*znum_dbl};
	std::vector<mpfr> exact_mpfr = {mpfr("2.0")*xnum_mpfr, mpfr("2.0")*ynum_mpfr, mpfr("2.0")*znum_mpfr};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() / exact_dbl[1].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() / exact_dbl[1].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}





BOOST_AUTO_TEST_CASE(diff_sin_lx_squared_times_yl){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = sin(x*y);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{-1,-1,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	
	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	std::vector<dbl> exact_dbl = {cos(xnum_dbl*ynum_dbl)*ynum_dbl, cos(xnum_dbl*ynum_dbl)*xnum_dbl, 0.0};
	std::vector<mpfr> exact_mpfr = {cos(xnum_mpfr*ynum_mpfr)*ynum_mpfr,
		cos(xnum_mpfr*ynum_mpfr)*xnum_mpfr, mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() / exact_dbl[0].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() / exact_dbl[0].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() / exact_mpfr[0].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() / exact_mpfr[0].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() / exact_dbl[1].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() / exact_dbl[1].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() / exact_mpfr[1].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() / exact_mpfr[1].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_cos_lx_squaredl){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = cos(x^2);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{-1,0,0};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),0);

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	std::vector<dbl> exact_dbl = {-2.0*sin(pow(xnum_dbl,2.0))*xnum_dbl, 0.0, 0.0};
	std::vector<mpfr> exact_mpfr = {mpfr("-2.0")*sin(pow(xnum_mpfr,mpfr("2.0")))*xnum_mpfr,mpfr("0.0"), mpfr("0.0")};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() / exact_dbl[0].real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() / exact_dbl[0].imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() / exact_mpfr[0].real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() / exact_mpfr[0].imag() -1) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(diff_tan_lx_over_zl){
	using mpfr_float = boost::multiprecision::mpfr_float;
	boost::multiprecision::mpfr_float::default_precision(DIFFERENTIATE_TREE_TEST_MPFR_DEFAULT_DIGITS);

	std::string str = "function f; variable_group x,y,z; f = tan(x/z);";

	bertini::System sys;
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bertini::SystemParser<std::string::const_iterator> S;
	phrase_parse(iter, end, S, boost::spirit::ascii::space, sys);

	var_dbl << xnum_dbl, ynum_dbl, znum_dbl;
	var_mpfr << xnum_mpfr, ynum_mpfr, znum_mpfr;
	sys.SetVariables<dbl>(var_dbl);
	sys.SetVariables<mpfr>(var_mpfr);

	auto func = sys.Function(0);
	auto vars = sys.Variables();
	auto JFunc = std::make_shared<Jacobian>(func->Differentiate());

	std::vector<int> multidegree{-1,0,-1};
	bool multidegree_ok = multidegree==func->MultiDegree(vars);
	BOOST_CHECK(multidegree_ok);

	BOOST_CHECK_EQUAL(func->Degree(vars[0]),-1);
	BOOST_CHECK_EQUAL(func->Degree(vars[1]),0);
	BOOST_CHECK_EQUAL(func->Degree(vars[2]),-1);

	

	BOOST_CHECK_EQUAL(func->Degree(vars), -1);

	std::vector<dbl> exact_dbl = {1.0/( znum_dbl*pow( cos(xnum_dbl/znum_dbl), 2.0 ) ), 0.0, -xnum_dbl/( pow(znum_dbl, 2.0)*pow( cos(xnum_dbl/znum_dbl), 2.0 ) )};
	std::vector<mpfr> exact_mpfr = {mpfr("1.0")/( znum_mpfr*pow( cos(xnum_mpfr/znum_mpfr), mpfr("2.0") ) ), mpfr("0.0"), -xnum_mpfr/( pow(znum_mpfr, mpfr("2.0"))*pow( cos(xnum_mpfr/znum_mpfr), mpfr("2.0") ) )};

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).real() - exact_dbl[0].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[0]).imag() - exact_dbl[0].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).real() - exact_mpfr[0].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[0]).imag() - exact_mpfr[0].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).real() - exact_dbl[1].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[1]).imag() - exact_dbl[1].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).real() - exact_mpfr[1].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[1]).imag() - exact_mpfr[1].imag() ) < threshold_clearance_mp);

	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).real() - exact_dbl[2].real() ) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<dbl>(vars[2]).imag() - exact_dbl[2].imag()) < threshold_clearance_d);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).real() - exact_mpfr[2].real() ) < threshold_clearance_mp);
	BOOST_CHECK(fabs(JFunc->EvalJ<mpfr>(vars[2]).imag() - exact_mpfr[2].imag() ) < threshold_clearance_mp);
}





BOOST_AUTO_TEST_CASE(arcsine_differentiate)
{
	std::shared_ptr<Variable> x = std::make_shared<Variable>("x");
	auto N = asin(pow(x,2)+1);
	auto J = std::make_shared<Jacobian>(N->Differentiate());

	x->set_current_value<dbl>(xnum_dbl);
	x->set_current_value<mpfr>(bertini::complex(xstr_real,xstr_imag));

	//(2*x)/(1 - (x^2 + 1)^2)^(1/2)
	dbl exact_dbl = 2.0*xnum_dbl / pow(1.0 - pow((xnum_dbl*xnum_dbl + 1.0),2),0.5);
	mpfr exact_mpfr = bertini::complex(2.0)*xnum_mpfr / pow(bertini::complex(1.0) - pow(xnum_mpfr*xnum_mpfr + bertini::complex(1.0),2),mpfr(0.5));

	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).real() / exact_dbl.real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).imag() / exact_dbl.imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).real() / exact_mpfr.real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).imag() / exact_mpfr.imag() -1) < threshold_clearance_mp);
}


BOOST_AUTO_TEST_CASE(arccosine_differentiate)
{
	std::shared_ptr<Variable> x = std::make_shared<Variable>("x");
	auto N = acos(pow(x,2)+1);
	auto J = std::make_shared<Jacobian>(N->Differentiate());

	x->set_current_value<dbl>(xnum_dbl);
	x->set_current_value<mpfr>(bertini::complex(xstr_real,xstr_imag));

	dbl exact_dbl = -2.0*xnum_dbl / pow(1.0 - pow((xnum_dbl*xnum_dbl + 1.0),2),0.5);
	mpfr exact_mpfr = -bertini::complex(2.0)*xnum_mpfr / pow(bertini::complex(1.0) - pow(xnum_mpfr*xnum_mpfr + bertini::complex(1.0),2),mpfr(0.5));

	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).real() / exact_dbl.real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).imag() / exact_dbl.imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).real() / exact_mpfr.real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).imag() / exact_mpfr.imag() -1) < threshold_clearance_mp);
}

BOOST_AUTO_TEST_CASE(arctangent_differentiate)
{
	std::shared_ptr<Variable> x = std::make_shared<Variable>("x");
	auto N = atan(pow(x,2)+1);
	auto J = std::make_shared<Jacobian>(N->Differentiate());


	x->set_current_value<dbl>(xnum_dbl);
	x->set_current_value<mpfr>(bertini::complex(xstr_real,xstr_imag));

	//(2*x)/((x^2 + 1)^2 + 1)
	dbl exact_dbl = 2.0*xnum_dbl / ( pow((xnum_dbl*xnum_dbl + 1.0),2) + 1.0);
	mpfr exact_mpfr = bertini::complex(2.0)*xnum_mpfr / ( pow((xnum_mpfr*xnum_mpfr + bertini::complex(1.0)),2) + bertini::complex(1.0));

	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).real() / exact_dbl.real() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<dbl>(x).imag() / exact_dbl.imag() -1) < threshold_clearance_d);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).real() / exact_mpfr.real() -1) < threshold_clearance_mp);
	BOOST_CHECK(fabs(J->EvalJ<mpfr>(x).imag() / exact_mpfr.imag() -1) < threshold_clearance_mp);
}





BOOST_AUTO_TEST_SUITE_END()