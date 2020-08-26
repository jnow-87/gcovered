/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef STATISTICS_H
#define STATISTICS_H


/* prototypes */
void stat_header(void);
void stat_line(file_data_t *covdata);

int stat_check_thresholds(file_data_t *covdata);


#endif // STATISTICS_H
