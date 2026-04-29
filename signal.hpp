/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 20:10:35 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/29 20:13:47 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <csignal>
class Signal
{
	public:
		static volatile sig_atomic_t runnin;
		static void reginster_singal();
		static void handler(int);
};

