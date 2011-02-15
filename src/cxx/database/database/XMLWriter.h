/**
 * @file src/cxx/database/database/XMLWriter.h
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a>
 *
 * @brief An XML Writer for a Dataset
 */

#ifndef TORCH5SPRO_DATABASE_XML_WRITER_H 
#define TORCH5SPRO_DATABASE_XML_WRITER_H

#include <string>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "database/Dataset.h"
#include "database/Arrayset.h"
#include "database/Array.h"
#include "database/Relationset.h"

namespace db = Torch::database;

namespace Torch {   
  /**
   * \ingroup libdatabase_api
   * @{
   *
   */
  namespace database {

    //Some promises
    //class Dataset;

    namespace detail {
      /**
       * @brief The main class for the XML parser
       */
      class XMLWriter {
        public:
          /**
           * @brief Constructor
           */
          XMLWriter();

          /**
           * @brief Destructor
           */
          ~XMLWriter();

          /**
           * @brief Write a Dataset to an XML file.
           */
          void write(const char *filename, const Dataset& dataset);

        private:
          /**
           * @brief Return an XML node containing an Arrayset
           */
          xmlNodePtr writeArrayset( xmlDocPtr doc, 
            size_t id, boost::shared_ptr<const db::Arrayset> a, 
            int precision=10, bool scientific=false);
          /**
           * @brief Return an XML node containing an Array
           */
          xmlNodePtr writeArray( xmlDocPtr doc, 
              size_t id, const db::Array a,
            int precision=10, bool scientific=false);
          /**
           * @brief Write the (casted) data in the given stringstrean
           */
          template <typename T, int D> void writeData( 
              std::stringstream& content, const blitz::Array<T,D> b) 
          {
            const T* data = b.data();
            for(blitz::sizeType i=0; i<b.numElements(); ++i)
              content << " " << data[i];
          }

          /**
           * @brief Return an XML node containing a Relationset
           */
//        xmlNodePtr writeRelationset( xmlDocPtr doc, const Relationset& r,
//            bool content_inline);
          /**
           * @brief Return an XML node containing a Rule
           */
//        xmlNodePtr writeRule( xmlDocPtr doc, const Rule& r);
          /**
           * @brief Return an XML node containing a Relation
           */
//        xmlNodePtr writeRelation( xmlDocPtr doc, const Relation& r);
          /**
           * @brief Return an XML node containing a Member
           */
//        xmlNodePtr writeMember( xmlDocPtr doc, const Member& m);
      };

    }
  }

  /**
   * @}
   */
}

#endif /* TORCH5SPRO_DATABASE_XML_WRITER_H */

