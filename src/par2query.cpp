#include <libpar2/par2repairer.h>

namespace par2 { namespace query {

class Par2Querier
	:
		protected Par2Repairer
{
public:
	Par2Querier();
	~Par2Querier();

	Result Process( const CommandLine &commandline, bool dorepair );

private:
	
};

Result Par2Repairer::Process(const CommandLine &commandline, bool dorepair)
{
	// What noiselevel are we using
	noiselevel = commandline.GetNoiseLevel();

	// Get filesnames from the command line
	string par2filename = commandline.GetParFilename();
	const list<CommandLine::ExtraFile> &extrafiles = commandline.GetExtraFiles();

	// Determine the searchpath from the location of the main PAR2 file
	string name;
	DiskFile::SplitFilename(par2filename, searchpath, name);

	// Load packets from the main PAR2 file
	if (!LoadPacketsFromFile(searchpath + name))
		return eLogicError;

	// Load packets from other PAR2 files with names based on the original PAR2 file
	if (!LoadPacketsFromOtherFiles(par2filename))
		return eLogicError;

	// Load packets from any other PAR2 files whose names are given on the command line
	if (!LoadPacketsFromExtraFiles(extrafiles))
		return eLogicError;

	if (noiselevel > CommandLine::nlQuiet)
		cout << endl;

	// Check that the packets are consistent and discard any that are not
	if (!CheckPacketConsistency())
		return eInsufficientCriticalData;

	// Use the information in the main packet to get the source files
	// into the correct order and determine their filenames
	if (!CreateSourceFileList())
		return eLogicError;

	// Determine the total number of DataBlocks for the recoverable source files
	// The allocate the DataBlocks and assign them to each source file
	if (!AllocateSourceBlocks())
		return eLogicError;

	// Create a verification hash table for all files for which we have not
	// found a complete version of the file and for which we have
	// a verification packet
	if (!PrepareVerificationHashTable())
		return eLogicError;

	// Compute the table for the sliding CRC computation
	if (!ComputeWindowTable())
		return eLogicError;

	if (noiselevel > CommandLine::nlQuiet)
		cout << endl << "Verifying source files:" << endl << endl;

	// Attempt to verify all of the source files
	if (!VerifySourceFiles())
		return eFileIOError;

	if (completefilecount<mainpacket->RecoverableFileCount())
	{
		if (noiselevel > CommandLine::nlQuiet)
			cout << endl << "Scanning extra files:" << endl << endl;

		// Scan any extra files specified on the command line
		if (!VerifyExtraFiles(extrafiles))
			return eLogicError;
	}

	// Find out how much data we have found
	UpdateVerificationResults();

	if (noiselevel > CommandLine::nlSilent)
		cout << endl;

	// Check the verification results and report the results
	if (!CheckVerificationResults())
		return eRepairNotPossible;

	// Are any of the files incomplete
	if (completefilecount<mainpacket->RecoverableFileCount())
	{
		// Do we want to carry out a repair
		if (dorepair)
		{
			if (noiselevel > CommandLine::nlSilent)
				cout << endl;

			// Rename any damaged or missnamed target files.
			if (!RenameTargetFiles())
				return eFileIOError;

			// Are we still missing any files
			if (completefilecount<mainpacket->RecoverableFileCount())
			{
				// Work out which files are being repaired, create them, and allocate
				// target DataBlocks to them, and remember them for later verification.
				if (!CreateTargetFiles())
					return eFileIOError;

				// Work out which data blocks are available, which need to be copied
				// directly to the output, and which need to be recreated, and compute
				// the appropriate Reed Solomon matrix.
				if (!ComputeRSmatrix())
				{
					// Delete all of the partly reconstructed files
					DeleteIncompleteTargetFiles();
					return eFileIOError;
				}

				if (noiselevel > CommandLine::nlSilent)
					cout << endl;

				// Allocate memory buffers for reading and writing data to disk.
				if (!AllocateBuffers(commandline.GetMemoryLimit()))
				{
					// Delete all of the partly reconstructed files
					DeleteIncompleteTargetFiles();
					return eMemoryError;
				}

				// Set the total amount of data to be processed.
				progress = 0;
				totaldata = blocksize * sourceblockcount * (missingblockcount > 0 ? missingblockcount : 1);

				// Start at an offset of 0 within a block.
				u64 blockoffset = 0;
				while (blockoffset < blocksize) // Continue until the end of the block.
				{
					// Work out how much data to process this time.
					size_t blocklength = (size_t)min((u64)chunksize, blocksize-blockoffset);

					// Read source data, process it through the RS matrix and write it to disk.
					if (!ProcessData(blockoffset, blocklength))
					{
						// Delete all of the partly reconstructed files
						DeleteIncompleteTargetFiles();
						return eFileIOError;
					}

					// Advance to the need offset within each block
					blockoffset += blocklength;
				}

				if (noiselevel > CommandLine::nlSilent)
					cout << endl << "Verifying repaired files:" << endl << endl;

				// Verify that all of the reconstructed target files are now correct
				if (!VerifyTargetFiles())
				{
					// Delete all of the partly reconstructed files
					DeleteIncompleteTargetFiles();
					return eFileIOError;
				}
			}

			// Are all of the target files now complete?
			if (completefilecount<mainpacket->RecoverableFileCount())
			{
				cerr << "Repair Failed." << endl;
				return eRepairFailed;
			}
			else
			{
				if (noiselevel > CommandLine::nlSilent)
					cout << endl << "Repair complete." << endl;
			}
		}
		else
		{
			return eRepairPossible;
		}
	}

	return eSuccess;
}


} }

int main( int argc, char *argv[] )
{
	try
	{
		
	}
	catch( const std::exception &e )
	{
		std::cout << "Caught Exception : " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch( ... )
	{
		std::cout << "Caught Exception : " << "UNKOWN" << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;	
}
