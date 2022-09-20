/*
 * custom_string.h
 *
 *  Created on: 2022. 9. 20.
 *      Author: Sujin
 */

#ifndef SRC_CUSTOM_STRING_H_
#define SRC_CUSTOM_STRING_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <gtk/gtk.h>

using namespace std;

static inline vector<guint> string_to_hex_array(string input)
{
    vector<guint> v_result;
    istringstream iss(input);             // istringstream에 str을 담는다.
    string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (getline(iss, buffer, ' ')) {
        istringstream iss_temp(buffer);
        guint result;
        iss_temp >> std::hex >> result;
        v_result.push_back(result);               // 절삭된 문자열을 vector에 저장
//		g_print("%X\n", result);
    }

    return v_result;
}

static inline gint string_to_hex_array(string input, char * output, gint max)
{
    istringstream iss(input);	// istringstream에 str을 담는다.
    string buffer;				// 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼
    gint cnt = 0;

    if((max == 0) || (output == 0))
    {
    	return 0;
    }

    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (getline(iss, buffer, ' ')) {
        istringstream iss_temp(buffer);
        guint result;
        iss_temp >> std::hex >> result;
        output[cnt] = (result);
//        g_print("%X\n", result);
        cnt++;
        if(cnt >= max)
        {
        	break;
        }
    }

    return cnt;
}

static inline string hex_array_to_string(char * input, gint length)
{
	string output;
    string buffer;

    if((input == 0) || (length == 0))
    {
    	return buffer;
    }

    for(int cnt=0; cnt<length; cnt++)
    {
    	char temp_buff[4] = {0};
    	sprintf(temp_buff, "%02X ", input[cnt]);
//    	string temp = std::to_string(input[cnt]);
    	buffer.append(temp_buff);
    }

	buffer.append("\n");
    return buffer;
}





#endif /* SRC_CUSTOM_STRING_H_ */
