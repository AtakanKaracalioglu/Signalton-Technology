#define _POSIX_C_SOURCE 200809L

#include "process_sound.h"

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <iostream>
#include <sstream>

using namespace std;



int main()
{
	const char *DIR = "./sound/";
	char *in_fname;
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	///////////////////////
	try {

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "ghost");
		/* Connect to the MySQL test database */
		con->setSchema("mydb");

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT * from mydb.wav_test_2;");

	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		//cout << "(" << EXAMPLE_FUNCTION << ") on line " << LINE << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}




	int retval;
	const char  *out_fname[2] = {NULL, NULL}, *list_fname = NULL;
	SNDFILE *in_sf = NULL, *out_sf[2] = {NULL, NULL};
	SF_INFO in_info = {0}, out_info[2];
	FILE *list_file = NULL;
	int mode, frame_ms = 10;
	Fvad *vad = NULL;
	int returnval = 0;
	long frames[2] = {0, 0};
	long segments[2] = {0, 0};
	int last_id = -1;

	std::ostringstream stringStream;
	std::string query_str;
	/*
	 * create fvad instance
	 */
	vad = fvad_new();
	if (!vad) {
		fprintf(stderr, "out of memory\n");
		goto fail;
	}
	returnval = fvad_set_mode(vad, 3);
	if(returnval < 0)
	{
		std::cout<<"Mod could not be set"<<std::endl;
	}


while(true)
{
	try
	{
		stringStream.str("");
		stringStream << "SELECT * from mydb.wav_test_2 where id > " << last_id << ";";
		res = stmt->executeQuery(stringStream.str());

	}
	catch (sql::SQLException &e)
	{
		cout << "# ERR: SQLException in " << __FILE__;
		//cout << "(" << EXAMPLE_FUNCTION << ") on line " << LINE << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	while(res->next())
	{
		last_id = std::stoi(res->getString("id"));

		in_fname = new char[res->getString("name").length() + strlen(DIR) + 1];
		strcpy(in_fname, DIR);
		strcat(in_fname, res->getString("name").c_str());
		in_fname[res->getString("name").length() + strlen(DIR)] = '\0';

		//cout << endl << res->getString("name").length() + strlen(DIR) + 1 << endl;
		//cout << endl << in_fname << endl;
		/*
		 * open and check input file
		 */
		in_sf = sf_open(in_fname, SFM_READ, &in_info);
		if (!in_sf) {
			fprintf(stderr, "Cannot open input file '%s': %s\n", in_fname, sf_strerror(NULL));
			goto fail;
		}

		if (in_info.channels != 1) {
			fprintf(stderr, "only single-channel wav files supported; input file has %d channels\n", in_info.channels);
			goto fail;
		}

		if (fvad_set_sample_rate(vad, in_info.samplerate) < 0) {
			fprintf(stderr, "invalid sample rate: %d Hz\n", in_info.samplerate);
			goto fail;
		}


		frames[0] = 0;
		frames[1] = 0;
		segments[0] = 0;
		segments[1] = 0;

		if (!process_sf(in_sf, vad,
				(size_t)in_info.samplerate / 1000 * frame_ms,list_file, frames, segments))
			goto fail;
		/*
		printf("voice detected in %ld of %ld frames (%.2f%%)\n",
			frames[1], frames[0] + frames[1],
			frames[0] + frames[1] ?
				100.0 * ((double)frames[1] / (frames[0] + frames[1])) : 0.0);
		printf("%ld voice segments, average length %.2f frames\n",
			segments[1], segments[1] ? (double)frames[1] / segments[1] : 0.0);
		printf("%ld non-voice segments, average length %.2f frames\n",
			segments[0], segments[0] ? (double)frames[0] / segments[0] : 0.0);
 	 	 */
		cout << "In sound file " << in_fname << " -> Voice Activity: " << (100 * (double)frames[1] / (frames[1] + frames[0])) << "%" << endl;

		stringStream.str("");
		stringStream << "UPDATE mydb.wav_test_2 set voice_seg=" << segments[1] << ", non_voice_seg =" << segments[0] \
				<< ", voice_length=" << frames[1] << ", non_voice_length=" << frames[0] << " where id=" << res->getString("id") << ";";
		query_str = stringStream.str();
		try
		{
			stmt->executeQuery(query_str);
		}
		catch(sql::SQLException &e)
		{
			2 == 2; // burayı değiştir
		}


		/*
		 * cleanup
		 */
		success:
		retval = EXIT_SUCCESS;
		goto end;

		argfail:
		//fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);

		std::cout  << "Wrong initilization"<<std::endl;
		fail:
		retval = EXIT_FAILURE;
		goto end;

		end:
		delete in_fname;

	}

	delete res;
}

	if (in_sf) sf_close(in_sf);
	for (int i = 0; i < 2; i++)
		if (out_sf[i]) sf_close(out_sf[i]);
	if (list_file) fclose(list_file);
	if (vad) fvad_free(vad);

	delete stmt;
	delete con;

	return retval;
}
