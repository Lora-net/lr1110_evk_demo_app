"""
Define job file reader class

 Revised BSD License
 Copyright Semtech Corporation 2020. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Semtech corporation nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

from json import load
from .Job import Job


class JobReader:
    INFINITE_LOOPS_KEY = "infinite_loops"
    JOBS_KEY = "jobs"
    SCAN_INTERVAL_KEY = "scan_interval"

    def __init__(self):
        self.jobs = list()
        self.infinite_execution = False
        self.scan_interval = 0

    def read_from_file(self, filename):
        with open(filename, "r") as f:
            configuration_raw = load(f)

        job_descriptions = [job for job in configuration_raw[JobReader.JOBS_KEY]]
        self.infinite_execution = configuration_raw[JobReader.INFINITE_LOOPS_KEY]
        self.scan_interval = configuration_raw[JobReader.SCAN_INTERVAL_KEY]

        for job_id, job_desc in enumerate(job_descriptions):
            job = JobReader.job_from_json(job_desc, job_id)
            self.jobs.extend([job] * job.n_iterations)

    @staticmethod
    def job_from_json(job_json, job_id):
        job = Job.from_dict(job_json)
        job.id = job_id
        return job
