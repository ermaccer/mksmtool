#pragma once


struct afs_header {
	int header;
	int numFiles;
};

struct afs_dictionary {
	int offset;
	int size;
};

struct afs_entry {
	int offset;
	int size;
};

struct afs_dictionary_entry {
	char name[32];
	short year;
	short month;
	short day;
	short hour;
	short min;
	short sec;
	int size;
};