/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 23:51:31 by mcutura           #+#    #+#             */
/*   Updated: 2024/06/02 14:25:56 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>

class Log
{
	public:
		static Log& getInstance();
		Log();
		virtual ~Log();

		enum e_loglevel
		{
			DEBUG,
			INFO,
			WARN,
			ERROR
		};

		void set_output(std::ostream *stream, bool is_file);
		void set_verbosity(e_loglevel const &verbosity);
		template <typename T>
		Log &operator<<(T const &obj)
		{
			if (!this->out_ || this->verbosity_curr_ < this->verbosity_)
				return *this;
			(*this->out_) << obj;
			return *this;
		}
		Log &operator<<(e_loglevel const &level);
		Log &operator<<(std::ostream &(*os)(std::ostream &));

	private:
		std::ostream	*out_;
		bool			is_file_;
		e_loglevel		verbosity_;
		e_loglevel		verbosity_curr_;

		void timestamp();

		/* no copy */
		Log(Log const &rhs);
		Log &operator=(Log const &rhs);
};

const char* const CYAN = "\033[36m";
const char* const RESET = "\033[0m";
const char* const RED = "\033[31m";
const char* const YELLOW = "\033[33m";
const char* const GREEN = "\033[32m";
const char* const MAGENTA = "\033[35m";

namespace d3
{
	extern Log	log;
}

void ERROR(char const *tag, char const *msg);
void WARN(char const *tag, char const *msg);
void INFO(char const *tag, char const *msg);

#endif // LOG_HPP
