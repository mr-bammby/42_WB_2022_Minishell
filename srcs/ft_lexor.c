#include "../incl/minishell.h"

/*
**	@brief Function used for comment check from standard input
**	@param char **args. If args containes # everything afterwards is being removed
**	@return No Return Value.
**	@todo /
*/

void ft_comment_check(char **args)
{
	int i;
	int s_quote_flag;
	int d_quote_flag;
	char *temp;

	i = 0;
	s_quote_flag = 0;
	d_quote_flag = 0;
	while ((*args)[i])
	{
		if ((*args)[i] == FT_SINGLE_QUOTE)
			s_quote_flag++;
		if ((*args)[i] == FT_DOUBLE_QUOTE)
			d_quote_flag++;
		if ((*args)[i] == FT_HASHTAG)
		{
			if (i == 0)
				break;
			if (s_quote_flag % 2 == 0 && d_quote_flag % 2 == 0)
				if ((*args)[i - 1] == FT_SPACE || (*args)[i - 1] == FT_PIPE || (*args)[i - 1] == FT_LESSER || (*args)[i - 1] == FT_GREATER)
					break;
		}
		i++;
	}
	temp = ft_substr(*args, 0, i);
	free(*args);
	*args = temp;
}

char *ft_getenv(char *str)
{
	int	search_len;
	t_env_var *temp_env;
	t_list *temp;

	search_len = ft_strlen(str);
	if (str[0] == '?' && search_len == 1)
	{
		return g_access.last_return;
	}
	temp = (t_list *)(*(g_access.env));
	if (!str)
		return "NOINPUT";
	while (temp)
	{
		temp_env = (t_env_var *)(temp->content);
		if (search_len == (int)(ft_strlen(temp_env->name) - 1))
			if (!(ft_strncmp(str, temp_env->name, search_len)))
				return (temp_env->value);
		temp = temp->next;
	}
	return "NOSTRING";
}
/*
**	env variables can be alphanumberic characters, it can be underscore,
**	equal  sign can be inside the value, but cant be inside the name
*/

void ft_env_check(char **args)
{
	int i;
	int j;
	int s_quote_flag;
	int d_quote_flag;
	char *temp1;
	char *temp0;
	char *temp2;

	i = 0;
	j = 0;
	s_quote_flag = 0;
	d_quote_flag = 0;
	while ((*args)[i])
	{
		if ((*args)[i] == FT_SINGLE_QUOTE)
			if (d_quote_flag % 2 == 0)
				s_quote_flag++;
		if ((*args)[i] == FT_DOUBLE_QUOTE)
			if (s_quote_flag % 2 == 0)
				d_quote_flag++;
		if ((*args)[i] == FT_DOLLAR_SIGN)
		{
			if (s_quote_flag % 2 == 0)
			{
				j = i + 1;
				while (ft_isalnum((*args)[j]) || (*args)[j] == FT_UNDERSCORE)
				{
					j++;
					if (ft_isdigit((*args)[j - 1]))
						break;
				}
				if (j == i + 1)
				{
					if ((*args)[j] == '*' || (*args)[j] == '@' || (*args)[j] == '#' ||
					 	(*args)[j] == '?' || (*args)[j] == '-' || (*args)[j] == '$' ||
						(*args)[j] == '!')
						j++;
					else
					{
						i++;
						continue;
					}
				}
				temp1 = ft_substr(*args, i + 1, j - i - 1);
				temp0 = ft_getenv(temp1);
				free(temp1);
				(*args)[i] = '\0';
				temp1 = ft_strjoin(*args, "\"");
				temp2 = ft_strjoin(temp1, temp0);
				free(temp1);
				temp1 = ft_strjoin(temp2, "\"");
				free(temp2);
				temp0 = ft_strjoin(temp1, &((*args)[j]));
				i = ft_strlen(temp1);
				free(*args);
				*args = temp0;
				free(temp1);
				i--;
			}
		}
		i++;
	}
}

int	lexor(void)
{
	int		i;
	int		begining;
	char	last;
	int		flag;
	char	*args;
	char	*current_str;

	args = g_access.read_line2lexor;
	i = 0;
	begining = 0;
	last = FT_SPACE;
	flag = 0;
	if (FT_LEXOR_COMMENT)
		printf("\n***********BEFORE COMMENT CHECK********** \n%s\n and the length is %ld\n", args, ft_strlen(args));
	ft_comment_check(&args);
	if (FT_LEXOR_COMMENT)
		printf("************AFTER COMMENT CHECK********** \n%s\n and the length is %ld\n", args, ft_strlen(args));
	ft_env_check(&args);
	current_str = NULL;
	while (args[i] != '\0')
	{
		if (last == FT_SPACE && args[i] != FT_SPACE)
		{
			begining = i;
		}
		else if (last != FT_SPACE && args[i] == FT_SPACE)
			ft_lex_space_handler(&current_str, args, i, begining);
		if (args[i] == FT_PIPE || args[i] == FT_GREATER || \
			args[i] == FT_LESSER)
		{
			ft_lex_operand_handler(&current_str, args, begining, i);
			begining = i + 1;
		}
		if (args[i] == FT_SINGLE_QUOTE)
		{
			flag = ft_lex_single_quote_handler(&current_str, args, begining, i);
			if (flag == -1)
				break;
			i = i + flag + 1;
			begining = i + 1;
		}
		if (args[i] == FT_DOUBLE_QUOTE)
		{
			flag = ft_lex_double_quote_handler(&current_str, args, begining, i);
			if (flag == -1)
				break;
			i = i + flag + 1; //we move counter I to the next letter from the next quote
			begining = i + 1; // begining of the next word should be after the first quote
		}
		last = args[i];
		i++;
	}
	if(args[i - 1] != FT_SPACE && i > 0 && flag != -1)
	{
		ft_lex_string_reminder_handler(&current_str, args, begining, i);
	}
	if (FT_LEXOR_COMMENT)
	{
		printf("We are in lexor %s\n", args);
		print_list(*g_access.lexor2parser);
		printf("Flag is %d\n", flag);
	}
	if (flag == -1)
		ft_free_list(*g_access.lexor2parser);
	free(args);
	args = NULL;
	return flag;
}