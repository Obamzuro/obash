/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/21 13:19:59 by obamzuro          #+#    #+#             */
/*   Updated: 2018/08/23 19:24:11 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/21sh.h"

int main(int argc, char **argv)
{
	int		ret;
	int		i;
	
	i = 0;
	while (i < 10)
	{
		write(3, "woop", 4);
		++i;
	}
//	while (1)
//		;
}
