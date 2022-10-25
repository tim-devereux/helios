#pragma once

#include <filems/write/comps/LasVectorialSyncFileMeasurementWriter.h>
#include <filems/write/comps/MultiLasSyncFileWriter.h>

#include <vector>
#include <memory>

namespace helios { namespace filems{

using std::vector;
using std::shared_ptr;
using std::make_shared;

/**
 * @author Alberto M. Esmoris Pena
 * @version 1.0
 * @brief Concrete class specializing MultiLasSyncFileWriter to write vectors
 *  of measurements to LAS files supporting concurrent multiple output streams
 *
 * @see filems:::MultiLasSyncFileWriter
 * @see filems::LasMeasurementWriteStrategy
 * @see Measurement
 * @see LasSyncFileMeasurementWriter
 */
class LasMultiVectorialSyncFileMeasurementWriter :
    public MultiLasSyncFileWriter<
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
     *  ( filems::MultiLasSyncFileWriter::writeStrategy )
     *
     */
    vector<LasMeasurementWriteStrategy> lmws;

public:
    // ***  CONSTRUCTION / DESTRUCTION  *** //
    // ************************************ //
    /**
     * @brief LAS multi-vectorial synchronous file measurement vector writer
     *  constructor
     * @see filems::MultiLasSyncFileWriter::MultiLasSyncFileWriter
     */
    explicit LasMultiVectorialSyncFileMeasurementWriter(
        vector<std::string> const &path,
        bool const compress,
        vector<double> const &scaleFactor,
        vector<glm::dvec3> const &offset,
        vector<double> const &minIntensity,
        vector<double> const &deltaIntensity,
        bool const createWriter = true
    ) :
        MultiLasSyncFileWriter<
            vector<Measurement> const &,
            glm::dvec3 const &
        >(
            path,
            compress,
            scaleFactor,
            offset,
            minIntensity,
            deltaIntensity,
            createWriter
        )
    {
        // Extract variables of interest
        size_t const nStreams = path.size();
        // Build measurement write strategies
        for(size_t i = 0 ; i < nStreams ; ++i){
            lmws.push_back(LasMeasurementWriteStrategy(
                *lw[i],
                lws[i].lp,
                lws[i].scaleFactorInverse,
                lws[i].offset,
                lws[i].minIntensity,
                lws[i].maxIntensity,
                lws[i].intensityCoefficient,
                lws[i].ewAttrStart,
                lws[i].fwiAttrStart,
                lws[i].hoiAttrStart,
                lws[i].ampAttrStart
            ));
        }
        // Build vectorial write strategies
        // WARNING : It must be done after building the measurement write
        // strategies to be wrapped by the vectorial strategy. If the vector
        // of measurement strategies is modified, then the references in the
        // vectorial strategy objects will be inconsistent.
        for(size_t i = 0 ; i < nStreams ; ++i){
            writeStrategy.push_back(make_shared<VectorialWriteStrategy<
                Measurement,
                glm::dvec3 const &
            >>(lmws[i]));
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