export type TTEntry = {
  id: string;
  start: Date;
  end: Date;
  allDay: boolean;
  description: string;
  backgroundColor: string;
  textColor: string;
  eventCategory: string;
  sites: string[];
}

export type TTDescription = {
  type: string;
  location: string;
  subject: string;
};

const decodeHTML = (raw: string) => raw.replace(/&#(\d+);/g, (_, dec) => String.fromCharCode(dec));

export const fetchTimetable = async (id: string, date: Date): Promise<TTEntry[]> => {
  const year = date.getFullYear();
  const month = date.getMonth() + 1;
  const url = `https://corsproxy.io/?${encodeURIComponent("https://edt.uvsq.fr/Home/GetCalendarData")}`;
  const body = `start=${year}-${month}-1&end=${year}-${month + 1}-1&resType=103&calview=month&federationIds%5B%5D=${id}&colourScheme=3`;
  return await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8", },
    body
  }).then(res => res.json())
}

export const parseDescription = (description: string) => {
  const [type, location, subject] = description.split("\r\n\r\n<br />\r\n\r\n").map(decodeHTML);
  return { type, location, subject };
}
