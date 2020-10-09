"""
Define entry point for NAV message parser

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

from argparse import ArgumentParser
import pkg_resources
from lr1110evk.NavParserFile.FileNavMessageParser import FileNavMessageParserInterpreter
from lr1110evk.NavParserFile.navParser.NavMessageParser import NavMessageParser


def entry_point_nav_parser_file():
    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser(description="Host companion for field tests of LR1110")
    parser.add_argument("--version", action="version", version=version)
    parser.add_argument(
        "-m",
        "--nav-message",
        help="The input_file is not a file to read but a NAV message to interpret",
        action="store_true",
        default=False,
    )
    parser.add_argument(
        "-o",
        "--output_file",
        help="File to write containing the interpreted NAV messages instead of printing on stout (does nothing if -m is provided)",
    )
    parser.add_argument(
        "input_file",
        help="File to read to extract and interpret NAV messages (or NAV message if -m is provided). Format of the file content is the same as the one provided by Lr1110FieldTest output",
    )
    args = parser.parse_args()

    if not args.nav_message:
        file_interpreter = FileNavMessageParserInterpreter(
            args.input_file, args.output_file
        )
        file_interpreter.parse_file()
    else:
        # In this case, args.input_file is not the result file to read, but a string representing a nav message to interpret
        nav_parsed = NavMessageParser.parse(args.input_file)
        sv_cn = NavMessageParser.get_cn_vs_from_parsed_message(nav_parsed)
        print("Satellite Name, C/N indicator")
        for sv_id, cn in sv_cn.items():
            print("{}, {}".format(sv_id, cn))
