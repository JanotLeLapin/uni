import { fetchTimetable, type TTEntry } from "@lib/timetable";
import { useEffect, useState } from "preact/hooks";

import "@style/timetable.css";

export default () => {
  const [timetable, setTimetable] = useState<TTEntry[]>([]);

  useEffect(() => {
    fetchTimetable("S2MITD-01", new Date(2024, 0, 23)).then(setTimetable);
  }, []);

  return (
    <div class="timetable">
      {timetable.map(tt => (
        <div
          key={tt.id}
          class="event"
          style={`--day: ${tt.start.getDay()}; --schedule: ${tt.start.getHours() - 7}`}>
            <p>{tt.description.subject}</p>
          </div>
      ))}
    </div>
  )
}
