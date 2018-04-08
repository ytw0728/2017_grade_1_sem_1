#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 2000
// variable
char inputs[INPUT_SIZE];	// 입력값 temp 변수
int exit_chk = 0; 				//프로그램 종료 조건

int num_var = 0;
char var_name[10] = { 0, };
int var_data[10][52];
int var_index[10];

int var_flag = 0, int_flag = 0, opt_flag = 0; // var 이거나 , int 이거나 opt 이거나
int add_sub_flag = 0, multi_flag = 0;		// 현재 덧셈 뺄셈 연산자가 있었는지, 곱하기 나 나누기 나 나머지 연산이 있었는지.
int last_flag = 0;			// 바로 이전에 어떤 형태의 문자가 입력되었는지.
int last_opt_flag = -1;		// 바로 이전의 이항 연산의 종류가 무엇이었는지 1 = 덧셈 뺄셈, 0 = 곱셈, 나눗셈, 나머지 연산
int is_ready_to_calc = -1;	// 이항 연산 준비가 되었는지에 대한 값, 1 = 다음 값 로드 후 즉각 연산 수행(*,/,%) , 0 = 연산 대기 중인 연산자 존재 (+,-)
char yet_used_opt[3] = { 0, };	// 아직 계산되지 않은 연산들		
int yet_used_opt_index = 0;		// 계산되지 않은 연산자들의 갯수.
int all_calc_ready = 0;			// 모든 연산자를 모두 역순으로 수행해도 되는지의 여부

int opr[3][52];
int opr_index[3] = { 0, };
int num_opr = 0;	// 피연산자 갯수

// 일반 함수 
void init();				// 연산을 위한 초기화 함수		/* 우준 */
int main_module();		// 입력값에 따른 처리 담당 함수		/* 시균 */
int input_string();		// 입력함수					/* 한샘 */

int calc();				// 문자열에 대한 세부적인 처리함수 및 복합연산 기본작업함수.				/* 태원 */
int calc_value( int index );	// 연산 중계 함수.													/* 태원 */
int add_sub(int index, int type);	// 덧셈 뺄셈을 통해서 모든 연산을 처리.							/* 태원 *
void print_value(int type);				//계산이 완료된 결과값을 출력. type == 1 -> 계산 결과, type == 2 -> var	/* 한샘 */
int is_first_bigger(int first, int second);	// 두 피연산자 큰수 배열 중 어떤 배열의 절댓값이 더 큰지	/* 우준 */

int set_opr(char ch);			// 피연산자를 설정하는 함수			/* 한샘 */
int set_opr_sign(char ch);		// 단항 연산자에 따른 피연산자의 부호를 결정해주는 함수	/* 시균 */

int exist_equal();		// 등호가 있는지 확인하는 함수		/* 우준 */
void print_var();		// 모든 변수 출력하기				/* 우준 */
int check_var(int equal_index);			// 변수 생성을 위한 입력값 검사 함수			/* 한샘 */
int make_var(char name, int first_index, int num_ints, int sign);		// 변수 생성			/* 한샘 */
int reset_var(int exist_var_index, int first_index, int num_ints, int sign);	// 변수 값 변경	 /* 시균 */

int set_var_opr(int index);		// 변수를 피연산자로 설정해주는 함수.		/* 시균 */
int exist_var(char name);	// 기존에 같은 이름의 변수가 있는지 확인하는 함수		/* 한샘 */

void print_error(int error_type);		// 에러 메시지 출력 함수.  // 1 = error , 2 = undefined variation , 3 = undefined command		/* 시균 */
void clear_page();		// 화면 지움		/* 우준 */

						// 자료구조 함수
char pop_front(int type);		// for queue	1 = inputs queue		/* 시균 */

int main() {
	int i, j;
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 51; j++) var_data[i][j] = -1;
		var_data[i][j] = 0;		// 마지막비트는 부호 & 0 은 +  1은 -1
	}
	printf("Starting...\n");
	while (!exit_chk) {
		init();
		printf("(input) ");
		input_string();
		main_module();
	}

	printf("Program END\n");
	return 0;
}
void init() {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 51; j++) opr[i][j] = -1;
		opr_index[i] = 0;
		opr[i][j] = 0;		// 마지막비트는 부호 & 0 은 +  1은 -1
	}
	num_opr = 0;


	var_flag = 0, int_flag = 0, opt_flag = 0;	// 현재 입력값의 종류
	add_sub_flag = 0, multi_flag = 0;		// 현재 이항 or 단항 연산자의 종류
	last_flag = 0;			// 마지막 입력값 종류
	last_opt_flag = -1;		// 마지막 이항 연산 종류
	
	is_ready_to_calc = -1;
	
	for (i = 0; i < 3; i++) {
		yet_used_opt[i] = 0; // 아직 계산되지 않은 연산들
	}
	yet_used_opt_index = 0;		// 아직 계산되지 않은 연산들의 인덱스

}

int main_module() {

	if( strlen(inputs) == 0) {
		print_error(0);
		return 0;
	}
	char end_var_check[4] = { 0, };
	char clear_check[6] = { 0, };

	int i;
	for (i = 0; i < 3; i++) end_var_check[i] = inputs[i];
	for (i = 0; i < 5; i++) clear_check[i] = inputs[i];
	if (!strcmp(end_var_check, "END")) {
		if (strlen(inputs) == 3) {
			exit_chk = 1;
		}
		else {
			print_error(3);
		}
		return 0;
	}
	if (!strcmp(end_var_check, "VAR")) {
		if (strlen(inputs) == 3) {
			print_var();
		}
		else {
			print_error(3);
		}
		return 0;
	}
	if (!strcmp(clear_check, "CLEAR")) {
		if (strlen(inputs) == 5) {
			clear_page();
		}
		else {
			print_error(3);
		}
		return 0;
	}
	int equal_index;
	if (equal_index = exist_equal()) {
		if (check_var(equal_index)) {
			print_error(1);
		}
		return 0;
	}
	calc();
	return 0;
}

int input_string() {
	int string_index = 0;
	char c = 1;
	int i;
	for (i = 0; i < INPUT_SIZE; i++) inputs[i] = 0;

	while (string_index  <INPUT_SIZE && c != '\n') {
		c = getchar();
		if (c == ' ' || c == '\n') continue;

		if ('a' <= c && c <= 'z') c -= ('a' - 'A');
		inputs[string_index++] = c;
	}
	return 0;
}


int calc() {
	char ch;
	int error = 0;
	int running = 1;
	while (running && !error && strlen(inputs)) {
		error = 0;
		ch = pop_front(1);
		// 문자확인
		
		last_flag = int_flag + var_flag * 2 + opt_flag * 3;		// last_flag = 1 - int, 2 - var , 3 - opt
		if ('0' <= ch && ch <= '9') {
			int_flag = 1;
			var_flag = 0;
			opt_flag = 0;
		}
		else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ) {		// + - 과 * / % 같은 우선순위
			if (ch == '+' || ch == '-') {
				add_sub_flag = 1;
				multi_flag = 0;
			}
			else {
				multi_flag = 1;
				add_sub_flag = 0;
			}



			if (int_flag) num_opr++;
			if (opt_flag || last_flag == 0) {
				if (add_sub_flag) {
					error = set_opr_sign(ch);
				}
				else {
					error = 1;
				}
			}
			opt_flag = 1;
			var_flag = 0;
			int_flag = 0;
		}
		else if (var_flag) {
			error = 1;
		}
		else if ('A' <= ch && ch <= 'Z') {
			var_flag = 1;
			opt_flag = 0;
			int_flag = 0;
		}
		else {
			error = 1;
		}


		if (error) {
			print_error(error);
			return 0;
		}
		// 문자처리
;		if (int_flag) {
			error = set_opr(ch);
		}
		if (var_flag) {
			int exist_var_index;
			if ( ( (exist_var_index = exist_var(ch)) != -1) && (last_flag == 0 || last_flag == 3 ) ) {
				error = set_var_opr(exist_var_index);
			}
			else if (exist_var_index == -1 ) { error = 2; }	
			else error = 1;
		}

		if (opt_flag) {
			if (last_flag == 1 || last_flag == 2) {
				if (yet_used_opt_index)	{
					if (last_opt_flag == -1);
					else if (last_opt_flag) {	// 전 이항 연산자가 + - 
						if (add_sub_flag) { // 현재 이항 연산자가 + -
							all_calc_ready = 1;
							error = calc_value(yet_used_opt_index);
							yet_used_opt[yet_used_opt_index++] = ch;
						}
						else {	// 현재 이항 연산자가 * / %
							yet_used_opt[yet_used_opt_index++] = ch;
							//error = calc_value(yet_used_opt_index);
						}
					}
					else {	// 전 이항 연산자가 * / %
						if (add_sub_flag) {	// 현재 이항 연산자가 + -
							all_calc_ready = 1;
						}
						else {
							all_calc_ready = 0;
						}
						error = calc_value(yet_used_opt_index);
						yet_used_opt[yet_used_opt_index++] = ch;
					}
				}
				else {
					yet_used_opt[yet_used_opt_index++] = ch;
				}
				if (last_flag == 1 || last_flag == 2) {
					last_opt_flag = add_sub_flag;
				}
			}
		}
	}

	if (error) {
		print_error(error);
		return 0;
	}

	if (strlen(inputs) == 0) {
		if (opt_flag == 1) {
			error = 1;		// 마지막 기호가 연산자 이면 error
		}
		else {
			if( int_flag ) num_opr++;	// 마지막이 int 일 때만 num_opr++ -> var일 경우 함수 내에서 자동적으로 처리해줌
			all_calc_ready = 1;
			error = calc_value(yet_used_opt_index);
		}
	}

	if (error) {
		print_error(error);
		return 0;
	}

	print_value(1);

	return 0;
}

int calc_value(int index ) {
	if (num_opr == 1 ){
		if (strlen(yet_used_opt) == 0) return 0;
		else return 1;
	}
	int error = 0;
	int i = index - 1;
	int until;
	if (all_calc_ready) {
		until = 0;
		all_calc_ready = 0;
	}
	else until = i;
	for (i; i >= until; i--) {
		switch (yet_used_opt[i]) {
			// add line
			case '+':	error = add_sub(i, 1); break;
			case '*':	error = add_sub(i, 2); break;
			// sub line
			case '-':	error = add_sub(i, 3); break;
			case '/':	error = add_sub(i, 4); break;
			case '%':	error = add_sub(i, 5); break;
			default: return 1; break;
		}
		if (error) {
			return 1;
		}
		yet_used_opt[i] = 0;
		yet_used_opt_index--;
	}
	return 0;
}

int add_sub(int index, int type) {
	int temp_data[52];
	int temp_index = 0;
	int index1 = 0, index2 = 0; // 피연산자 의 인덱스
	int first, second, result;
	int i;
	for (i = 0; i < 51; i++) temp_data[i] = -1;
	temp_data[51] = 0;
	first = num_opr - 2;
	second = num_opr - 1;
	result = first;

	index1 = opr_index[first];
	index2 = opr_index[second];

	index1--, index2--;

	if (type <= 2) {
		if (type == 1) {	// type == 1 더하기
			if (opr[first][51] == opr[second][51]) {
				if (index1 < index2) {
					int temp;
					temp = first;
					first = second;
					second = temp;

					temp = index1;
					index1 = index2;
					index2 = temp;
				}

				for (index1,index2; index1 >= 0 && temp_index < 51; index1--, index2--) {
					if (temp_data[temp_index] == -1) temp_data[temp_index] = 0;
					if (index2 <= -1) {
						temp_data[temp_index] += opr[first][index1];
					}
					else {
						temp_data[temp_index] += opr[first][index1] + opr[second][index2];
					}
					if (temp_data[temp_index] >= 10) {
						if (temp_data[temp_index + 1] == -1) temp_data[temp_index + 1] = 0;
						temp_data[temp_index + 1] += temp_data[temp_index] / 10;
						temp_data[temp_index] = temp_data[temp_index] % 10;
					}
					temp_index++;
				}

				if (temp_data[50] != -1) return 1;
				temp_data[51] = (opr[first][51] == 0) ? 0 : 1;
			}
			else{
				if ( is_first_bigger(first,second) ) {
					opr[second][51] = opr[first][51];
				}
				else {
					int temp;
					opr[first][51] = opr[second][51];
					for (i = 0; i < 51 && (opr[first][i] != -1 || opr[second][i] != -1); i++ ) {
						temp = opr[first][i];
						opr[first][i] = opr[second][i];
						opr[second][i] = temp;
					}
					temp = opr_index[first];
					opr_index[first] = opr_index[second];
					opr_index[second] = temp;
				}
				return add_sub(index, 3);		// 부호바꿔서 빼기 재호출
			}
		}
		else if( type == 2){	// type == 2 곱하기
			int chk_index = temp_index;
			int chk_index1 = index1;
			int chk_index2 = index2;
			for (index1; index1 >= 0 && temp_index + 1 < 51; index1--) {
				temp_index = chk_index + (chk_index1 - index1);
				for (index2 = chk_index2; index2 >= 0 && temp_index + 1 < 51; index2--) {
					if (temp_data[temp_index] == -1) temp_data[temp_index] = 0;
					temp_data[temp_index] += opr[first][index1] * opr[second][index2];
					while (temp_data[temp_index] >= 10) {
						if (temp_data[temp_index + 1] == -1)temp_data[temp_index + 1] = 0;
						temp_data[temp_index + 1] += temp_data[temp_index] / 10;
						temp_data[temp_index] %= 10;
					}
					temp_index++;
				}
			}
			if (temp_data[50] != -1) return 1;
			temp_data[51] = opr[first][51] ^ opr[second][51];
		}
		else return 1;
	}
	else {
		if (type == 3) {	// 빼기
			if (opr[first][51] == opr[second][51]) {
				int chk_bigger = is_first_bigger(first, second);
				if (chk_bigger == 2) {
					opr[result][0] = 0;
					opr[result + 1][0] = -1;
					int i;
					for (i = 1; i < 51 && (opr[result][i] != -1 || opr[result+1][i] != -1); i++) {
						opr[result][i] = -1;
						opr[result + 1][i] = -1;
					}

					opr_index[result] = 1;
					opr_index[result + 1] = 0;

					opr[result][51] = 0;
					opr[result + 1][51] = 0;

					num_opr--;
					return 0;
				}
				if ( chk_bigger == 0 ) {
					int temp;
					temp = index1;
					index1 = index2;
					index2 = temp;

					temp = first;
					first = second;
					second = temp;
				}
				int i = 0;
				for (i; i <= index2; i++) {
					if (opr[first][index1 - i] < opr[second][index2 - i]) {
						int t = index1-i-1;			// 값 빌려올 위치
						while (opr[first][t] <= 0 && t > 0) {		// first 값이 더 크므로 t의 해당 값이 -1 이 나올리 없다 즉, t =0 일때 최종값은 언제나 0보단 크다
							t--;
						}
						for (++t; t <= index1 - i ; t++) {
							opr[first][t - 1] -= 1;
							opr[first][t] += 10;
						}
						if (opr[first][0] == 0) {
							int t = 1;
							while (opr[first][t] != -1 && t <= 49) {
								opr[first][t - 1] = opr[first][t];
								t++;
							}
							t--;
							if( t <= 49 ) opr[first][t] = -1;

							index1--;
						}
						temp_data[temp_index++] = opr[first][index1-i] - opr[second][index2-i];
					}
					else {
						temp_data[temp_index++] = opr[first][index1-i] - opr[second][index2-i];
					}
				}
				i = ( index1 - index2 ) - 1;
				for (i; i >= 0; i--) temp_data[temp_index++] = opr[first][i];

				if( chk_bigger == 1 ) temp_data[51] = opr[first][51];
				else if (chk_bigger == 0) temp_data[51] = !opr[first][51];
			}
			else {
				opr[second][51] = !opr[second][51];
				return add_sub(index, 1);		// 부호 바꿔서 더하기 재호출
			}

		}
		else if (type == 4 || type == 5) {		// 나누기, 나머지
			if (opr_index[second] == 1 && opr[second][0] == 0) return 1; // error
			else if (opr_index[first] == 1 && opr[first][0] == 0) {
				int i;
				opr[result][0] = 0;
				opr[result + 1][0] = -1;
				for (i = 1; i < 51 && ( opr[result][i] != -1 || opr[result+1][i] != -1 ); i++) {
					opr[result+1][i] = opr[result][i] = -1;
				}
				opr_index[result] = 1;
				opr_index[result + 1] = 0;
				num_opr--;
				return 0;
			}

			int chk_bigger = is_first_bigger(first, second);
			if ( chk_bigger == 0 || chk_bigger == 2) {
				if (type == 4) {
					int quotient = (chk_bigger != 0) ? 1 : 0;
					opr[result][0] = quotient;
					opr[result + 1][0] = -1;
					opr[result][51] = (chk_bigger == 0)? 0 : opr[result][51] ^ opr[result + 1][51];
					opr[result + 1][51] = 0;
					int i;
					for (i = 1; i < 51 && (opr[result][i] != -1 || opr[result + 1][i] != -1); i++) {
						opr[result][i] = -1;
						opr[result + 1][i] = -1;
					}

					opr_index[result] = 1;
					opr_index[result + 1] = 0;
					num_opr--;
					return 0;
				}
				else if (type == 5) {
					if (chk_bigger == 2) {
						opr[result][0] = 0;
						opr[result + 1][0] = -1;
						opr[result][51] = (chk_bigger == 2) ? 0 : opr[result][51] ^ opr[result + 1][51];
						opr[result + 1][51] = 0;
						int i;
						for (i = 1; i < 51 && (opr[result][i] != -1 || opr[result + 1][i] != -1); i++) {
							opr[result][i] = -1;
							opr[result + 1][i] = -1;
						}
						opr_index[result] = 1;
						opr_index[result + 1] = 0;
					}
					else {
						opr[result][51] = opr[result][51] ^ opr[result + 1][51];
						opr[result + 1][51] = 0;
						for( i = 0; i< 51 && opr[result+1][i] != -1 ; i++) opr[result + 1][i] = -1;
						opr_index[result + 1] = 0;
					}
					num_opr--;
					return 0;
				}
			}
			else {
				int setting_index = opr_index[first] - opr_index[second];
				int quotient[50];
				int quotient_index = 0;
				for (i = 0; i < 50; i++) quotient[i] = -1;
				quotient[0] = 0;

				int second_size = opr_index[second];
				int until;
				for (until = 0; until <= setting_index; until++) {
					int cnt = 0;
					for ( i = second_size; i < second_size + setting_index; i++ ) {
						if( i >= second_size + setting_index - until ) opr[second][i] = -1;
						else {
							opr[second][i] = 0;
							cnt++;
						}
					}
					opr_index[second] = second_size + cnt;
					index2 = opr_index[second] - 1;
					while (is_first_bigger(first, second) != 0) {
						quotient[0]++;

						for (i = 0; i < 51; i++) temp_data[i] = -1;
						index1 = opr_index[first];
						index1--;

						int i = 0;
						for (i; i <= index2; i++) {
							if (opr[first][index1 - i] < opr[second][index2 - i]) {
								int t = index1 - i - 1;			// 값 빌려올 위치
								while (opr[first][t] <= 0 && t > 0) {
									t--;
								}
								for (++t; t <= index1 - i; t++) {
									opr[first][t - 1] -= 1;
									opr[first][t] += 10;
								}
								if (opr[first][0] == 0) {
									int t = 1;
									while (opr[first][t] != -1 && t <= 49) {
										opr[first][t - 1] = opr[first][t];
										t++;
									}
									t--;
									if (t <= 49) opr[first][t] = -1;

									index1--;
								}
								temp_data[temp_index++] = opr[first][index1 - i] - opr[second][index2 - i];
							}
							else {
								temp_data[temp_index++] = opr[first][index1 - i] - opr[second][index2 - i];
							}
						}
						i = (index1 - index2) - 1;
						for (i; i >= 0; i--) temp_data[temp_index++] = opr[first][i];

						temp_data[51] = opr[first][51];




						///////	opr[first] 로 재할당///////////////

						for (i = 0; i < 51 && opr[first][i] != -1; i++) {
							opr[first][i] = -1;
						}

						int length = 0;
						while (temp_data[length++] != -1);
						length -= 2;
						while (temp_data[length] == 0 && length > 0) {
							temp_data[length--] = -1;
						}

						int temp_length = length;
						opr_index[first] = length + 1;
						for (i = 0; i <= length; i++) {
							opr[first][i] = temp_data[temp_length--];
						}
						temp_index = 0;
						opr[first][51] = temp_data[51];
						///////////////////////////////////////
					}
					if (quotient_index >= 50) return 1;
					for (i = quotient_index; i >= 0; i--) { // 각 차례마다 기껏해야 quotient는 9가 최대 : 자릿수를 맞춰주기 때문
						quotient[i + 1] = quotient[i];
					}
					quotient[0] = 0;
					quotient_index++;
					// 최종 quotient 는 1 인덱스 까지이다. 0 인덱스에는 무의미한 0이 추가됨.
				}

				for (i = 0; i < 51 && temp_data[i] != -1; i++) temp_data[i] = -1;
				temp_data[51] = opr[first][51] ^ opr[second][51];
				if (type == 4) {
					for( i = quotient_index; i >= 1; i--){
						temp_data[i-1] = quotient[i];
					}
				}
				else if (type == 5) {
					int i = 0;
					while( i < 51 && opr[first][i] != -1) i++;
					temp_index = 0;
					for (i--; i >= 0; i--) temp_data[temp_index++] = opr[first][i];
				}
			}
		}
		else return 1;
	}


	for (i=0; i < 51 && ( opr[first][i] != -1 || opr[second][i] != -1); i++) {
		opr[first][i] = opr[second][i] = -1;
	}
	opr[first][51] = opr[second][51] = 0;
	
	opr_index[first] = 0;
	opr_index[second] = 0;

	int length = 0;
	while (temp_data[length++] != -1) ;
	length -= 2;
	while(temp_data[length] == 0 && length > 0) {
		temp_data[length--] = -1;
	}

	int temp_length = length;
	opr_index[result] = length+1;
	for (i = 0; i <= length; i++) {
		opr[result][i] = temp_data[temp_length--];
	}
	if (length == 0 && temp_data[0] == 0) temp_data[51] = 0;
	opr[result][51] = temp_data[51];

	opr_index[num_opr-1] = 0;
	num_opr--;
	return 0;
}

int is_first_bigger(int first, int second) {
	int index1 =0, index2=0;

	index1 = opr_index[first];
	index2 = opr_index[second];
	if (index1 > index2) return 1;	// 더 큽니다.
	else if (index1 < index2) return 0; // 더 작습니다.
	else {
		int i = 0, j = 0;
		for (i, j; i < index1, j < index2; i++,j++) {
			if (opr[first][i] > opr[second][j]) return 1;
			else if (opr[first][i] < opr[second][j]) return 0;
		}
	}

	return 2;	// 서로 같습니다.
}

int set_opr(char ch) {
	if (num_opr >= 3) {
		return 1;	// error
	}
	if (opr_index[num_opr] && opr[num_opr][0] == -1) {
		return 1;	// error
	}
		int i;
		if (opr_index[num_opr]< 50) {
			opr[num_opr][opr_index[num_opr]++] = ch - 48;
		}
		else {
			int i;
			for (i = 0; i < opr_index[num_opr]; i++) {
				opr[num_opr][i] = -1;
			}
			opr_index[num_opr] = 0;
			num_opr--;
			return 1;	// error
		}
	return 0; // no error
}
int set_var_opr(int exist_var_index) {

	if ( num_opr >= 3) return 1;

	int i;
	for (i = 0; i < var_index[exist_var_index]; i++) {
		opr[num_opr][i] = var_data[exist_var_index][i];
	}
	opr_index[num_opr] = i;
	if( opr[num_opr][51] != var_data[exist_var_index][51] ) opr[num_opr][51] = 1;
	else opr[num_opr][51] = 0;

	num_opr++;
	return 0;
}

int set_opr_sign(char ch) {
	if ( num_opr >= 3) return 1;
	else {
		if (ch == '-') opr[num_opr ][51] = opr[num_opr][51]^1;
	}
	return 0;
}

int exist_equal() {		// 한샘
	int chk = 0;
	int i;
	for (i = 0; i < strlen(inputs); i++) {
		if (inputs[i] == '=') {
			chk = i;
			break;
		}
	}
	return chk;
}

void print_value(int type) {
	if (type == 1) {
		while (opr[0][0] == 0 && opr_index[0] > 1) {
			int t = 1;
			while (opr[0][t] != -1 && t <= 49) {
				opr[0][t - 1] = opr[0][t];
				t++;
			}
			t--;
			if (t <= 49) opr[0][t] = -1;
			opr_index[0]--;
		}
		int i = 0;
		int length = 0;
		while (opr[0][length++] != -1 && length <= 50);

		printf("= ");
		if (opr[0][51] == 1) printf("-");
		while (opr[0][i] != -1 && i <= 49) {
			if ((length - (i + 1)) % 3 == 0 && i != 0) printf(",");
			printf("%d", opr[0][i++]);
		}

		printf("\n");
	}
	else if (type == 2) {
		int i = 0;
		for (i; i < num_var; i++) {
			int j = 0;
			printf(" %d. %c = ", i+1,var_name[i]);
			int length = 0;
			while (var_data[i][length++] != -1 && length <= 50);

			if (var_data[i][51] == 1) printf("-");
			while (var_data[i][j] != -1 && i <= 49) {
				if ((length - (j + 1)) % 3 == 0 && j != 0) printf(",");
				printf("%d", var_data[i][j++]);
			}

			printf("\n");
		}
		printf("\n");
	}
}

void print_var() {
	printf("\n[ The number of variable : %d ]\n", num_var);
	printf("[ Variables ]\n");
	print_value(2);
}
int check_var(int equal_index) {
	if (equal_index > 1) {
		return 1;
	}
	else {
		char name = inputs[0];
		if ('A' <= name && name <= 'Z') {
			int i, chk_int = 0;
			int first_index = 2;
			i = 2;
			int sign = 0;
			while (!('0' <= inputs[i] && inputs[i] <= '9')) {
				if (inputs[i] == '+' || inputs[i] == '-' ) {
					if (inputs[i] == '-') sign = sign ^ 1;
				}
				else return 1;
				i++;
			}
			first_index = i;
			for (i; i < strlen(inputs); i++) {
				if ('0' <= inputs[i] && inputs[i] <= '9') chk_int++;
				else return 1;
			}
			if (chk_int <= 50) {
				int exist_var_index;
				if ((exist_var_index = exist_var(name)) != -1) {
					return reset_var(exist_var_index, first_index, chk_int, sign);
				}
				else {
					return make_var(name, first_index, chk_int, sign);
				}
			}
			else {
				return 1;
			}
		}
		else return 1;
	}
	return 0;
}

int exist_var(char name) {
	int i;
	for (i = 0; i < num_var; i++) {
		if (var_name[i] == name) return i;
	}
	return -1;
}

int reset_var(int exist_var_index, int first_index, int num_ints, int sign) {
	int i;
	int index = 0;
	for (i = 0; i < 51; i++) var_data[exist_var_index][i] = -1;

	var_data[exist_var_index][51] = sign;
	for (i = 0; i < num_ints; i++) {
		var_data[exist_var_index][i] = inputs[i + first_index] - 48;
		index++;
	}
	var_index[exist_var_index] = index;

	while (var_data[exist_var_index][0] == 0 && var_index[num_var] > 1) {
		int t = 1;
		while (var_data[exist_var_index][t] != -1 && t <= 49) {
			var_data[exist_var_index][t - 1] = var_data[exist_var_index][t];
			t++;
		}
		t--;
		if (t <= 49) var_data[exist_var_index][t] = -1;
		var_index[exist_var_index]--;
	}

	printf("\n[ %c's value is changed. ]\n\n", var_name[exist_var_index]);
	return 0;
}
int make_var(char name, int first_index, int chk_int, int sign) {
	if ((num_var + 1) <= 10) {
		var_name[num_var] = name;
		var_data[num_var][51] = sign;
		int i;
		for (i = 0; i < chk_int; i++) {
			var_data[num_var][i] = inputs[i + first_index] - 48;
		}
		var_index[num_var] = i;

		while (var_data[num_var][0] == 0 && var_index[num_var] > 1 ) {
			int t = 1;
			while (var_data[num_var][t] != -1 && t <= 49) {
				var_data[num_var][t - 1] = var_data[num_var][t];
				t++;
			}
			t--;
			if (t <= 49) var_data[num_var][t] = -1;

			var_index[num_var]--;
		}
		num_var++;
	}
	else return 1;

	printf("\n[ Variable %c is saved. ]\n[ The number of Variable : %d ]\n\n", name, num_var);
	return 0;
}


void clear_page() {
	system("clear");
}
void print_error(int error_type) {
	if (error_type == 0) return; 		// 에러는 아니지만 특수한 케이스로 분리해 관리
	else if (error_type == 1 || error_type == 2 || error_type == 3) printf(" = error\n");
}


char pop_front(int type) {
	char ch;
	if (type == 1) {		// inputs pop
		ch = inputs[0];
		int i;
		for (i = 1; i < strlen(inputs); i++) {
			inputs[i - 1] = inputs[i];
		}
		inputs[i-1] = 0;

		return ch;
	}
	else if (type == 2) {
		if( strlen(yet_used_opt) ) {
			ch = yet_used_opt[0];
			int i;
			for (i = 1; i < strlen(yet_used_opt); i++) {
				yet_used_opt[i - 1] = yet_used_opt[i];
			}
			yet_used_opt[i - 1] = 0;

			return ch;
		}
		else return 1;
	}

	return 0;
}
