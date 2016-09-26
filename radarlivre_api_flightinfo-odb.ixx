// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

namespace odb
{
  // radarlivre_api_flightinfo
  //

  inline
  access::object_traits< ::radarlivre_api_flightinfo >::id_type
  access::object_traits< ::radarlivre_api_flightinfo >::
  id (const object_type& o)
  {
    return o.id;
  }

  inline
  void access::object_traits< ::radarlivre_api_flightinfo >::
  callback (database& db, object_type& x, callback_event e)
  {
    ODB_POTENTIALLY_UNUSED (db);
    ODB_POTENTIALLY_UNUSED (x);
    ODB_POTENTIALLY_UNUSED (e);
  }

  inline
  void access::object_traits< ::radarlivre_api_flightinfo >::
  callback (database& db, const object_type& x, callback_event e)
  {
    ODB_POTENTIALLY_UNUSED (db);
    ODB_POTENTIALLY_UNUSED (x);
    ODB_POTENTIALLY_UNUSED (e);
  }
}

namespace odb
{
  // radarlivre_api_flightinfo
  //

  inline
  void access::object_traits_impl< ::radarlivre_api_flightinfo, id_pgsql >::
  erase (database& db, const object_type& obj)
  {
    callback (db, obj, callback_event::pre_erase);
    erase (db, id (obj));
    callback (db, obj, callback_event::post_erase);
  }

  inline
  void access::object_traits_impl< ::radarlivre_api_flightinfo, id_pgsql >::
  load_ (statements_type& sts,
         object_type& obj,
         bool)
  {
    ODB_POTENTIALLY_UNUSED (sts);
    ODB_POTENTIALLY_UNUSED (obj);
  }
}

