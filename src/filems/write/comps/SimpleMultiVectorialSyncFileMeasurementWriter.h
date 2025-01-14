#pragma once

#include <filems/write/comps/SimpleMultiSyncFileWriter.h>

#include <memory>
#include <vector>
#include <string>

namespace helios { namespace filems{

/**
 * @author Alberto M. Esmoris PEna
 * @version 1.0
 * @brief Concrete class specializing a SimpleMultiSyncFileWriter to write
 *  vectors of measurements directly to files with support for concurrent
 *  multiple output streams
 */
class SimpleMultiVectorialSyncFileMeasurementWriter :
    public SimpleMultiSyncFileWriter<
        vector<Measurement> const &,
        glm::dvec3 const &
    >
{
protected:
    // ***  ATTRIBUTES  *** //
    // ******************** //
    /**
     * @brief The measurement write strategies that are wrapped by the main
     *  write strategies in a vectorial fashion
     *  ( filems::SimpleMultiSyncFileWriter::writeStrategy )
     */
    vector<DirectMeasurementWriteStrategy> dmws;

public:
    // ***  CONSTRUCTION / DESTRUCTION  *** //
    // ************************************ //
    /**
     * @brief Simple synchronous file measurement vector multi-writer
     *  constructor
     * @see filems::SimpleMultiSyncFileWriter::SimpleMultiSyncFileWriter
     */
    explicit SimpleMultiVectorialSyncFileMeasurementWriter(
        vector<string> const &path,
        std::ios_base::openmode om = std::ios_base::app
    ) :
        SimpleMultiSyncFileWriter<
            vector<Measurement> const &,
            glm::dvec3 const &
        >(path, om)
    {
        // Build measurement write strategies
        buildMeasurementWriteStrategies();
        // Build vectorial write strategies
        // WARNING : It must be done after building the measurement write
        // strategies to be wrapped by the vectorial strategy. If the vector
        // of measurement strategies is modified, then the references in the
        // vectorial strategy objects will be inconsistent
        buildVectorialWriteStrategies();
    }
    virtual ~SimpleMultiVectorialSyncFileMeasurementWriter() = default;

    // ***   B U I L D   *** //
    // ********************* //
    /**
     * @brief Build the measurement write strategies for the simple multi
     *  vectorial synchronous file measurement writer.
     */
    void buildMeasurementWriteStrategies(){
        // Build measurement write strategies
        size_t const nStreams = path.size();
        for(size_t i = 0 ; i < nStreams ; ++i){
            dmws.emplace_back(ofs[i]);
        }
    }
    /**
     * @brief Build the vectorial write strategies for the simple multi
     *  vectorial synchronous file measurement writer.
     *
     * <b>WARNING!</b> calling the buildMeasurementWriteStrategies() method
     *  will invalidate the state generated by buildVectorialWriteStrategies()
     */
    void buildVectorialWriteStrategies(){
        // Build vectorial write strategies
        size_t const nStreams = path.size();
        for(size_t i = 0 ; i < nStreams ; ++i){
            writeStrategy.push_back(make_shared<VectorialWriteStrategy<
                Measurement, glm::dvec3 const &
            >>(dmws[i]));
        }
    }

    // ***  W R I T E  *** //
    // ******************* //
    /**
     * @see MultiSyncFileWriter::indexFromWriteArgs
     */
    size_t indexFromWriteArgs(
        vector<Measurement> const &measurements,
    glm::dvec3 const &offset
    ) override {return measurements[0].devIdx;}
};

}}