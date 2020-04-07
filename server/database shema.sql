create schema if not exists Weather_Station collate latin1_swedish_ci;

create table if not exists Stats
(
	ID int auto_increment,
	Date datetime null,
	InsideTemp float null,
	OutsideTemp float null,
	Pressure float null,
	InsideHumidity float null,
	OutsideHumidity float null,
	constraint table_name_ID_uindex
		unique (ID)
);

alter table table_name
	add primary key (ID);