/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 15:05:22 by obamzuro          #+#    #+#             */
/*   Updated: 2018/09/09 22:18:43 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "21sh.h"

int			handle_commands(char **args,
		char ***env)
{
	int		i;

//	if (!(args = ft_strsplit2(line, " \t"))[0])
//	{
//		free_double_arr(args);
//		return ;
//	}
//	replace_env_variable(args, *env);
	extern t_comm_corr g_commands[AM_COMMANDS];
	i = -1;
	while (++i < AM_COMMANDS)
	{
		if (!ft_strncmp(g_commands[i].comm, args[0],
					ft_strlen(g_commands[i].comm) + 1))
		{
			g_commands[i].func(args, env);
			break ;
		}
	}
	if (i == AM_COMMANDS)
		return (0);
	return (1);
}

void				history_append(char *command, t_history *history)
{
	char	*temp;
	int		i;

//	if (!command)
//		return ;
	if (history->last == AM_HISTORY)
	{
//		write(1, "1", 1);
		i = -1;
		free(history->commands[0]);
		while (++i < AM_HISTORY - 1)
			history->commands[i] = history->commands[i + 1];
		history->commands[AM_HISTORY - 1] = 0;
		--history->last;
	}
	if (!history->commands[history->last])
	{
//		write(1, "2", 1);
		history->commands[history->last] = ft_strdup(command);
	}
	else
	{
//		write(1, "3", 1);
		temp = history->commands[history->last];
		history->commands[history->last] = ft_strjoin(history->commands[history->last], command);
		free(temp);
//		free(command);
	}
}

int					free_ast(t_ast *ast)
{
	if (!ast)
		return (1);
	if (ast->type == REDIRECTION && ft_strequ((char *)ast->content, "<<"))
		free(((t_binary_token *)(ast->right->content))->right);
	free_ast(ast->left);
	free_ast(ast->right);
	free(ast->content);
	free(ast);
	return (1);
}

static void	get_cursor_position2(int cursorpos[2])
{
	char	temp;

	read(2, &temp, 1);
	while (temp >= '0' && temp <= '9')
	{
		cursorpos[1] = cursorpos[1] * 10 + (temp - '0');
		read(2, &temp, 1);
	}
	if (temp != 'R')
		return ;
}

static void	get_cursor_position(int cursorpos[2])
{
	char	temp[16];

	cursorpos[0] = 0;
	cursorpos[1] = 0;
	write(2, "\x1B[6n", 4);
	read(2, &temp, 16);
	cursorpos[0] = ft_atoi(temp + 2);
	cursorpos[1] = ft_atoi(temp + 3 + ft_nbr_size(cursorpos[0]));
//	read(2, &temp, 1);
//	if (temp != '\x1b')
//		return ;
//	read(2, &temp, 1);
//	if (temp != '[')
//		return ;
//	read(2, &temp, 1);
//	while (temp >= '0' && temp <= '9')
//	{
//		cursorpos[0] = cursorpos[0] * 10 + (temp - '0');
//		read(2, &temp, 1);
//	}
//	if (temp != ';')
//		return ;
//	get_cursor_position2(cursorpos);
}

void				line_editing_left(t_lineeditor *lineeditor)
{
	int				i;

	if (lineeditor->seek)
	{
		if (lineeditor->curpos[1] == 0)
		{
			if (lineeditor->buffer[lineeditor->seek - 1] == '\n')
			{
				--lineeditor->curpos[0];
				ft_putstr(tgetstr("up", 0));
				i = lineeditor->seek - 2;
				while (i >= 0 && lineeditor->buffer[i] != '\n')
				{
					ft_printf(tgetstr("nd", 0));
					++lineeditor->curpos[1];
					--i;
				}
				if (i < 0)
				{
					ft_printf(tgetstr("nd", 0));
					ft_printf(tgetstr("nd", 0));
					ft_printf(tgetstr("nd", 0));
					lineeditor->curpos[1] += 3;
				}
			}
			else
			{
				ft_printf(tgetstr("le", 0));
				--lineeditor->curpos[0];
				lineeditor->curpos[1] = lineeditor->ws.ws_col - 1;
			}
		}
		else
		{
			ft_printf(tgetstr("le", 0));
			--lineeditor->curpos[1];
		}
		--lineeditor->seek;
	}
}

void				line_editing_left_notmove(t_lineeditor *lineeditor)
{
	int				i;

	if (lineeditor->seek)
	{
		if (lineeditor->curpos[1] == 0)
		{
			if (lineeditor->buffer[lineeditor->seek - 1] == '\n')
			{
				--lineeditor->curpos[0];
				i = lineeditor->seek - 2;
				while (i >= 0 && lineeditor->buffer[i] != '\n')
				{
					++lineeditor->curpos[1];
					--i;
				}
				if (i < 0)
					lineeditor->curpos[1] += 3;
			}
			else
			{
				--lineeditor->curpos[0];
				lineeditor->curpos[1] = lineeditor->ws.ws_col - 1;
			}
		}
		else
			--lineeditor->curpos[1];
		--lineeditor->seek;
	}
}

void				write_line_chcurpos(t_lineeditor *lineeditor)
{
	int		amlinebreak;
	char	*str;
	int		linewidth;
	int		nextlinebreak;

	str = lineeditor->buffer;
	while (*str)
	{
		if (str != lineeditor->buffer)
			lineeditor->curpos[1] = 0;
		nextlinebreak = ft_ccount(str, '\n');
		linewidth = /*lineeditor->curpos[1] + */3 + nextlinebreak;
		if (linewidth % lineeditor->ws.ws_col == 0)
			lineeditor->curpos[0] += linewidth / lineeditor->ws.ws_col;
		else
		{
			lineeditor->curpos[0] += linewidth / (lineeditor->ws.ws_col);
			if (lineeditor->curpos[0] >= lineeditor->ws.ws_row)
				lineeditor->curpos[0] = lineeditor->ws.ws_row - 1;
		}
		lineeditor->curpos[1] = linewidth % (lineeditor->ws.ws_col);
		str += nextlinebreak;
	}
}

void				write_line(t_lineeditor *lineeditor)
{
	int		offset;
	int		left;
	int		right;
	char	*reverse;

	reverse = tgetstr("mr", 0);
	left = lineeditor->selected[0];
	right = lineeditor->selected[1];
	if (left > right)
	{
		left = lineeditor->selected[1];
		right = lineeditor->selected[0];
	}
	offset = right - left;
	if (lineeditor->selectedmode)
	{
		ft_printf("%.*s%s%.*s%s%s", left, lineeditor->buffer,
			reverse, offset, lineeditor->buffer + left,
			DEFAULT, lineeditor->buffer + left + offset);
		//ft_putnbr(left);
//		ft_printf("%s", lineeditor->buffer);
		//ft_putstr(lineeditor->buffer);
	}
	else
		ft_putstr(lineeditor->buffer);
	write_line_chcurpos(lineeditor);
}

void				left_shift_cursor(int amount, t_lineeditor *lineeditor)
{
	int				i;

	i = 0;
	while (i < amount)
	{
		line_editing_left_notmove(lineeditor);
		++i;
	}
	if (lineeditor->curpos[0] < 0)
	{
		lineeditor->curpos[0] = 0;
		lineeditor->curpos[1] = 0;
	}
	ft_putstr(tgoto(tgetstr("cm", 0), lineeditor->curpos[1],
			lineeditor->curpos[0]));
}

void				line_editing_altup(t_lineeditor *lineeditor)
{
	struct winsize	ws;
	int				i;
	int				curpos[2];

	get_cursor_position(curpos);
	if (ioctl(0, TIOCGWINSZ, &ws) == -1)
		return ;
	i = 0;
	while (lineeditor->seek && i < ws.ws_col)
	{
		line_editing_left(lineeditor);
		--(lineeditor->seek);
		if (lineeditor->buffer[lineeditor->seek] == '\n')
			break ;
		++i;
	}
}

void				line_editing_altdown(t_lineeditor *lineeditor)
{
	struct winsize	ws;
	int				i;
	int				j;
	int				buflen;
	int				curpos[2];

	ft_bzero(curpos, 2);
	get_cursor_position(curpos);
	buflen = ft_strlen(lineeditor->buffer);
	if (ioctl(0, TIOCGWINSZ, &ws) == -1)
		return ;
	i = 0;
	while (lineeditor->seek != buflen && i < ws.ws_col)
	{
		j = -1;
		if (ws.ws_col == curpos[1])
		{
			++curpos[0];
			curpos[1] = 0;
			ft_putstr(tgoto(tgetstr("cm", 0), 0, curpos[0] - 1));
		}
		else
			ft_printf(tgetstr("nd", 0));
		++(lineeditor->seek);
		++curpos[1];
		++i;
//		if (lineeditor->buffer[lineeditor->seek] == '\n')
//			break ;
	}
}

void				line_editing_altright(t_lineeditor *lineeditor)
{
	struct winsize	ws;
	int				curpos[2];
	int				buflen;

	buflen = ft_strlen(lineeditor->buffer);
	if (lineeditor->seek != buflen)
	{
		if (ioctl(0, TIOCGWINSZ, &ws) == -1)
			return ;
		ft_bzero(curpos, 2);
		get_cursor_position(curpos);
		while (lineeditor->seek != buflen && (lineeditor->buffer)[lineeditor->seek] == ' ')
		{
			if (ws.ws_col == curpos[1])
			{
				++curpos[0];
				curpos[1] = 0;
				ft_putstr(tgoto(tgetstr("cm", 0), 0, curpos[0] - 1));
			}
			else
				ft_printf(tgetstr("nd", 0));
			++(lineeditor->seek);
			++curpos[1];
		}
		while (lineeditor->seek != buflen && (lineeditor->buffer)[lineeditor->seek] != ' ')
		{
			if (ws.ws_col == curpos[1])
			{
				++curpos[0];
				curpos[1] = 0;
				ft_putstr(tgoto(tgetstr("cm", 0), 0, curpos[0] - 1));
			}
			else
				ft_printf(tgetstr("nd", 0));
			++(lineeditor->seek);
			++curpos[1];
		}
	}
}

void				line_editing_right(t_lineeditor *lineeditor)
{
	int				buflen;

	buflen = ft_strlen(lineeditor->buffer);
	if (lineeditor->seek != buflen)
	{
		if (lineeditor->ws.ws_col - 1 == lineeditor->curpos[1])
		{
			++lineeditor->curpos[0];
			ft_putstr(tgoto(tgetstr("cm", 0), 0, lineeditor->curpos[0]));
			lineeditor->curpos[1] = 0;
		}
		else
		{
			ft_printf(tgetstr("nd", 0));
			++lineeditor->curpos[1];
		}
		++(lineeditor->seek);
	}
}

void				line_editing_help(t_lineeditor *lineeditor)
{
	ft_printf("\ncursorpos[0]=%d  ws_col=%d\ncursorpos[1]=%d  ws_row=%d\nseek=%d\n",
			lineeditor->curpos[0], lineeditor->ws.ws_col,
			lineeditor->curpos[1], lineeditor->ws.ws_row,
			lineeditor->seek);
//	int i = 0;
//
//	while (i < 5)
//	{
//		ft_printf("%s\n", history->commands[i]);
//		++i;
//	}
//	ft_printf("cur = %d\nlast = %d\nseek = %d\n", history->current, history->last, lineeditor->seek);
//	return (1);
}

//void				line_editing_altx(t_lineeditor *lineeditor)
//{
//	int		offset;
//	int		left;
//	int		right;
//	char	*temp;
//	char	*temp2;
//
//	if (!lineeditor->selectedmode)
//		return ;
//	lineeditor->selectedmode = 0;
//	left = lineeditor->selected[0];
//	right = lineeditor->selected[1];
//	if (left > right)
//	{
//		left = lineeditor->selected[1];
//		right = lineeditor->selected[0];
//	}
//	offset = right - left;
//	if (lineeditor->cpbuf)
//		free(lineeditor->cpbuf);
//	lineeditor->cpbuf = ft_strsub(lineeditor->buffer, left, offset);
//	temp = ft_strsub(lineeditor->buffer, 0, left);
//	temp2 = lineeditor->buffer;
//	lineeditor->buffer = ft_strjoin(temp, lineeditor->buffer + right);
//	free(temp2);
//	free(temp);
//	left_shift_cursor(0, - lineeditor->seek, lineeditor);
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("cd", 0));
//	write(1, "$> ", 3);
//	if (lineeditor->seek > ft_strlen(lineeditor->buffer))
//		lineeditor->seek = ft_strlen(lineeditor->buffer);
//	write_line(lineeditor);
//	left_shift_cursor(lineeditor->buffer, lineeditor->seek, lineeditor);
//	lineeditor->selected[0] = -1;
//	lineeditor->selected[1] = -1;
//}
//
//void				line_editing_altv(t_lineeditor *lineeditor)
//{
//	char	*temp;
//
//	if (!lineeditor->cpbuf)
//		return ;
//	temp = lineeditor->buffer;
//	lineeditor->buffer = ft_strjoin_inner(lineeditor->buffer,
//			lineeditor->cpbuf, lineeditor->seek);
//	free(temp);
//	left_shift_cursor(0, - lineeditor->seek, lineeditor);
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("le", 0));
//	ft_putstr(tgetstr("cd", 0));
//	write(1, "$> ", 3);
//	write_line(lineeditor);
//	lineeditor->seek += ft_strlen(lineeditor->cpbuf);
//	left_shift_cursor(lineeditor->buffer, lineeditor->seek, lineeditor);
//}

void				line_editing_altc(t_lineeditor *lineeditor)
{
	int		offset;
	int		left;
	int		right;
	char	*temp;

	if (!lineeditor->selectedmode)
		return ;
	left = lineeditor->selected[0];
	right = lineeditor->selected[1];
	if (left > right)
	{
		left = lineeditor->selected[1];
		right = lineeditor->selected[0];
	}
	offset = right - left;
	if (lineeditor->cpbuf)
		free(lineeditor->cpbuf);
	lineeditor->cpbuf = ft_strsub(lineeditor->buffer, left, offset);
}

int					line_editing(t_lineeditor *lineeditor, t_history *history)
{
	int					i;
	extern t_esc_corr	g_esc[AM_ESC];

	i = 0;
	while (i < AM_ESC)
	{
		if (ft_strequ(lineeditor->letter, g_esc[i].str))
		{
			g_esc[i].func(lineeditor);
			return (1);
		}
		++i;
	}
	return (0);
}

//	char	*temp;
//	int		i;
//	int		buflen;
//	int		curpos[2];
//
//	buflen = ft_strlen(lineeditor->buffer);
//	if (!ft_strequ(lineeditor->letter, SHIFTLEFT) && 
//		!ft_strequ(lineeditor->letter, SHIFTRIGHT) &&
//		!ft_strequ(lineeditor->letter, ALTX) &&
//		!ft_strequ(lineeditor->letter, ALTC) &&
//		lineeditor->selectedmode)
//	{
//		lineeditor->selectedmode = 0;
//		lineeditor->selected[0] = -1;
//		lineeditor->selected[1] = -1;
//		left_shift_cursor(0, - lineeditor->seek, lineeditor);
//		ft_putstr(tgetstr("le", 0));
//		ft_putstr(tgetstr("le", 0));
//		ft_putstr(tgetstr("le", 0));
//		ft_putstr(tgetstr("cd", 0));
//		write(1, "$> ", 3);
//		write_line(lineeditor);
//		left_shift_cursor(lineeditor->buffer, lineeditor->seek, lineeditor);
//	}
//	if (ft_strequ(lineeditor->letter, LEFT))
//	{
//		get_cursor_position(curpos);
//		line_editing_left(lineeditor);
//		--(lineeditor->seek);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, RIGHT))
//	{
//		line_editing_right(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, BACKSPACE))
//	{
//		if (lineeditor->seek)
//		{
//			//ft_putstr(tgoto(tgetstr("cm", 0), lineeditor->cursorpos[1] - 1, lineeditor->cursorpos[0] - 1));
//			(lineeditor->buffer)[lineeditor->seek - 1] = 0;
//			temp = lineeditor->buffer;
//			lineeditor->buffer = ft_strjoin(lineeditor->buffer, lineeditor->buffer + lineeditor->seek);
//			free(temp);
//			left_shift_cursor(0, - lineeditor->seek, lineeditor);
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("cd", 0));
//			write(1, "$> ", 3);
//			write_line(lineeditor);
//			left_shift_cursor(lineeditor->buffer, lineeditor->seek - 1, lineeditor);
//			--(lineeditor->seek);
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTLEFT))
//	{
//		if (lineeditor->seek)
//		{
//			get_cursor_position(curpos);
//			--(lineeditor->seek);
//			line_editing_left(lineeditor);
//			while (lineeditor->seek > 0 && (lineeditor->buffer)[lineeditor->seek] == ' ')
//			{
//				line_editing_left(lineeditor);
//				--(lineeditor->seek);
//			}
//			while (lineeditor->seek > 0 && (lineeditor->buffer)[lineeditor->seek - 1] != ' ')
//			{
//				line_editing_left(lineeditor);
//				--(lineeditor->seek);
//			}
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTRIGHT))
//	{
//		line_editing_altright(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, HOME))
//	{
//		while (lineeditor->seek)
//		{
//			get_cursor_position(curpos);
//			--(lineeditor->seek);
//			line_editing_left(lineeditor);
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, END))
//	{
//		while (lineeditor->seek != buflen)
//			line_editing_altdown(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, UP))
//	{
//		if (history->current)
//		{
//		//	ft_putstr(tgoto(tgetstr("cm", 0), lineeditor->cursorpos[1] - 1, lineeditor->cursorpos[0] - 1));
//			left_shift_cursor(0, - lineeditor->seek, lineeditor);
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("cd", 0));
//			write(1, "$> ", 3);
//			if (!lineeditor->is_history_searched && history->current == history->last)
//			{
//				if (history->last == AM_HISTORY)
//					--history->current;
//				history_append(lineeditor->buffer, history);
//				++history->last;
//			}
//			--history->current;
//			free(lineeditor->buffer);
//			lineeditor->buffer = ft_strdup(history->commands[history->current]);
//			write_line(lineeditor);
//			lineeditor->seek = ft_strlen(lineeditor->buffer);
//			lineeditor->is_history_searched = 1;
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, DOWN))
//	{
//		if (history->current < history->last - 1)
//		{
//		//	ft_putstr(tgoto(tgetstr("cm", 0), lineeditor->cursorpos[1] - 1, lineeditor->cursorpos[0] - 1));
//			left_shift_cursor(0, - lineeditor->seek - 3, lineeditor);
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("cd", 0));
//			write(1, "$> ", 3);
//			++history->current;
//			free(lineeditor->buffer);
//			lineeditor->buffer = ft_strdup(history->commands[history->current]);
//			write_line(lineeditor);
//			lineeditor->seek = ft_strlen(lineeditor->buffer);
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ESC))
//	{
//	}
//	else if (ft_strequ(lineeditor->letter, ALTUP))
//	{
//		line_editing_altup(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTDOWN))
//	{
//		line_editing_altdown(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, SHIFTLEFT))
//	{
//		if (lineeditor->seek)
//		{
//			get_cursor_position(curpos);
//			lineeditor->selectedmode = 1;
//			if (lineeditor->selected[1] == -1)
//				lineeditor->selected[1] = lineeditor->seek;
//			line_editing_left(lineeditor);
//			lineeditor->selected[0] = lineeditor->seek;
//			left_shift_cursor(0, - lineeditor->seek, lineeditor);
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("cd", 0));
//			write(1, "$> ", 3);
//			write_line(lineeditor);
//			left_shift_cursor(lineeditor->buffer, lineeditor->seek, lineeditor);
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, SHIFTRIGHT))
//	{
//		if (lineeditor->seek)
//		{
//			lineeditor->selectedmode = 1;
//			if (lineeditor->selected[1] == -1)
//				lineeditor->selected[1] = lineeditor->seek;
//			line_editing_right(lineeditor);
//			left_shift_cursor(0, - lineeditor->seek, lineeditor);
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("le", 0));
//			ft_putstr(tgetstr("cd", 0));
//			write(1, "$> ", 3);
//			lineeditor->selected[0] = lineeditor->seek;
//			write_line(lineeditor);
//			left_shift_cursor(lineeditor->buffer, lineeditor->seek, lineeditor);
//		}
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTX))
//	{
//		line_editing_altx(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTC))
//	{
//		line_editing_altc(lineeditor);
//		return (1);
//	}
//	else if (ft_strequ(lineeditor->letter, ALTV))
//	{
//		line_editing_altv(lineeditor);
//		return (1);
//	}
//	return (0);
//}

void				print_buffer(t_lineeditor *lineeditor)
{
	int		prevseek;
	int		buflen;

	buflen = ft_strlen(lineeditor->buffer);
	prevseek = lineeditor->seek;
	left_shift_cursor(lineeditor->seek, lineeditor);
	int a;
	//read(1, &a, 1);
	//get_cursor_position(lineeditor->curpos);
	ft_printf("%s%s%s", tgetstr("le", 0), tgetstr("le", 0), tgetstr("le", 0));
	//ft_putstr(tgetstr("le", 0));
	//ft_putstr(tgetstr("le", 0));
	write(1, "$> ", 3);
	write_line(lineeditor);
	//ft_printf("\ncursorpos[0]=%d  ws_col=%d\ncursorpos[1]=%d  ws_row=%d\nseek=%d\n",
	//		lineeditor->curpos[0], lineeditor->ws.ws_col,
	//		lineeditor->curpos[1], lineeditor->ws.ws_row,
	//		lineeditor->seek);
	//get_cursor_position(lineeditor->curpos);
	lineeditor->seek += buflen;
	left_shift_cursor(buflen - prevseek - 1, lineeditor);
}

char				*input_command(t_history *history)
{
	t_lineeditor	lineeditor;
	char			*temp;

	ft_bzero(&lineeditor, sizeof(lineeditor));
	if (ioctl(0, TIOCGWINSZ, &lineeditor.ws) == -1)
		return (NULL);
//	lineeditor.selected[0] = -1;
//	lineeditor.selected[1] = -1;
	get_cursor_position(lineeditor.curpos);
	--lineeditor.curpos[1];
	--lineeditor.curpos[0];
	while (1)
	{
		ft_bzero(lineeditor.letter, sizeof(lineeditor.letter));
		read(0, lineeditor.letter, sizeof(lineeditor.letter));
		if (line_editing(&lineeditor, history))
		{
//			if (ft_strequ(lineeditor.letter, BACKSPACE) && lineeditor.is_history_searched)
//			{
//				--history->last;
//				free(history->commands[history->last]);
//				history->commands[history->last] = 0;
//				lineeditor.is_history_searched = 0;
//				history->current = history->last;
//			}
			continue ;
		}
//		if (lineeditor.is_history_searched)
//		{
//			--history->last;
//			free(history->commands[history->last]);
//			history->commands[history->last] = 0;
//			lineeditor.is_history_searched = 0;
//			history->current = history->last;
//		}
		if (ft_strequ(lineeditor.letter, "\n"))
		{
			write(1, lineeditor.letter, sizeof(lineeditor.letter));
			break ;
		}
		temp = lineeditor.buffer;
//		write(1, lineeditor.letter, sizeof(lineeditor.letter));
		if (lineeditor.seek != ft_strlen(lineeditor.buffer))
		{
//			write(1, lineeditor.buffer + lineeditor.seek, ft_strlen(lineeditor.buffer) - lineeditor.seek);
//			left_shift_cursor(lineeditor.buffer, lineeditor.seek);
			lineeditor.buffer = ft_strjoin_inner(lineeditor.buffer, lineeditor.letter, lineeditor.seek);
		}
		else
  			lineeditor.buffer = ft_strjoin(lineeditor.buffer, lineeditor.letter);
//		ft_putstr(tgoto(tgetstr("cm", 0), lineeditor.cursorpos[1] - 1, lineeditor.cursorpos[0] - 1));
		free(temp);
		print_buffer(&lineeditor);
//		++lineeditor.seek;
//		++lineeditor.curpos[1];
	}
	free(lineeditor.cpbuf);
	return (lineeditor.buffer);
}

int					winch_handler(int sig)
{
	int	curpos[2];

	if (sig == SIGWINCH)
	{
		get_cursor_position(curpos);
	}
}

void				preparation(t_shell *shell)
{
	term_associate();
	shell->initfd.fdin = dup(0);
	shell->initfd.fdout = dup(1);
	shell->env = fill_env();
	ft_bzero(&shell->history, sizeof(shell->history));
//	iignal(SIGWINCH, winch_handler);
	//signal(SIGINT, int_handler);
}

int					main(void)
{
	char		*command;
	t_lexer		lexer;
	t_ast		*ast;
	t_shell		shell;

	preparation(&shell);
	while (1)
	{
		change_termios(&shell.initfd, 0);
		ft_bzero(&lexer, sizeof(t_lexer));
		ft_printf("$> ");
		if (!(command = input_command(&shell.history)))
			continue ;
		lexer_creating(command, &lexer, &shell);
		if (!lexer.tokens.len)
		{
			free_lexer(&lexer);
			free(shell.history.commands[shell.history.last]);
			shell.history.commands[shell.history.last] = 0;
			continue ;
		}
		if (!(ast = create_separator_ast(&lexer, 0, lexer.tokens.len - 1, 0, &shell)))
		{
			free(shell.history.commands[shell.history.last]);
			shell.history.commands[shell.history.last] = 0;
		}
		else
		{
//			if (shell.history.last != AM_HISTORY)
			++shell.history.last;
			shell.history.current = shell.history.last;
		}
		print_ast(ast);
		change_termios(&shell.initfd, 1);
		parse_ast(ast, &shell, 1);
		free_lexer(&lexer);
		free_ast(ast);
		system("leaks -quiet 21sh");
	}
	free_double_arr(shell.env);
	//TODO: close shell->initfd
	return (0);
}
